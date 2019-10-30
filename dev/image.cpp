// image.cpp - Image I/O.
#include "dev/image.hpp"

#include "dev/log.hpp"

#include <vector>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

namespace tcm {
namespace {
struct BaseState {
    explicit BaseState(std::string path);
    BaseState(const BaseState &) = delete;
    ~BaseState();
    BaseState &operator=(const BaseState &) = delete;

    bool Open();
    bool Commit();

    const std::string path;
    int fdes = -1;
};

BaseState::BaseState(std::string path) : path{path} {}

BaseState::~BaseState() {
    if (fdes != -1) {
        close(fdes);
        unlink(path.c_str());
    }
}

bool BaseState::Open() {
    fdes = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fdes == -1) {
        ErrorErrno(errno, "Could not create %s", path.c_str());
        return false;
    }
    return true;
}

bool BaseState::Commit() {
    int r = close(fdes);
    fdes = -1;
    if (r == -1) {
        int ecode = errno;
        unlink(path.c_str());
        ErrorErrno(ecode, "Could not write %s", path.c_str());
        return false;
    }
    return true;
}

} // namespace
} // namespace tcm

#if defined __APPLE__

#include <ApplicationServices/ApplicationServices.h>

namespace tcm {

namespace {

void FreeData(void *info, const void *data, size_t size) {
    (void)data;
    (void)size;
    (void)info;
}

struct State : BaseState {
    State(std::string path);
    ~State();

    CGColorSpaceRef color_space = nullptr;
    CGDataProviderRef data = nullptr;
    CGImageRef image = nullptr;
    CGDataConsumerRef consumer = nullptr;
    CGImageDestinationRef dest = nullptr;
};

State::State(std::string path) : BaseState{std::move(path)} {}

State::~State() {
    if (consumer != nullptr) {
        CFRelease(consumer);
    }
    if (image != nullptr) {
        CFRelease(image);
    }
    if (data != nullptr) {
        CFRelease(data);
    }
    if (color_space != nullptr) {
        CFRelease(color_space);
    }
}

size_t PutBytes(void *info, const void *buffer, size_t count) {
    State &st = *static_cast<State *>(info);
    ssize_t amt = write(st.fdes, buffer, count);
    if (amt < 0) {
        int ecode = errno;
        ErrorErrno(ecode, "Could not write to %s", st.path.c_str());
        return 0;
    }
    return amt;
}

void ReleaseConsumer(void *info) {
    (void)info;
}

} // namespace

bool WritePNG(const std::string &path, const void *data, int width,
              int height) {
    State st{path};
    st.color_space = CGColorSpaceCreateDeviceRGB();
    if (st.color_space == nullptr) {
        Error("CGColorSpaceCreateDeviceRGB failed");
        return false;
    }
    st.data = CGDataProviderCreateWithData(nullptr, data, width * height * 4,
                                           FreeData);
    if (st.data == nullptr) {
        Error("CGDataProviderCreateWithData failed");
        return false;
    }
    st.image = CGImageCreate(width, height, 8, 4 * 8, width * 4, st.color_space,
                             kCGImageAlphaNoneSkipFirst, st.data, nullptr,
                             false, kCGRenderingIntentDefault);
    if (st.image == nullptr) {
        Error("CGImageCreate failed");
        return false;
    }
    if (!st.Open()) {
        return false;
    }

    CGDataConsumerCallbacks callbacks;
    callbacks.putBytes = PutBytes;
    callbacks.releaseConsumer = ReleaseConsumer;
    st.consumer = CGDataConsumerCreate(&st, &callbacks);
    if (st.consumer == nullptr) {
        Error("CGDataConsumerCreate failed");
        return false;
    }

    st.dest = CGImageDestinationCreateWithDataConsumer(st.consumer, kUTTypePNG,
                                                       1, nullptr);
    if (st.dest == nullptr) {
        Error("CGImageDestinationCreateWithDataConsumer failed");
        return false;
    }
    CGImageDestinationAddImage(st.dest, st.image, nullptr);
    bool success = CGImageDestinationFinalize(st.dest);
    if (!success) {
        Error("CGImageDestinationFinalize failed");
        return false;
    }

    return st.Commit();
}

} // namespace tcm

#else /* __APPLE__ */

#include <png.h>

namespace tcm {

namespace {

struct State : BaseState {
    explicit State(std::string path);
    ~State();

    png_struct *png = nullptr;
    png_info *info = nullptr;
    std::vector<unsigned char *> rows;
};

State::State(std::string path) : BaseState{std::move(path)} {}

State::~State() {
    png_destroy_write_struct(&png, &info);
}

void UserError(png_struct *png, const char *msg) {
    const State &st = *static_cast<State *>(png_get_error_ptr(png));
    Error("Could not write %s: LibPNG: %s", st.path.c_str(), msg);
    longjmp(png_jmpbuf(png), 1);
}

void UserWarning(png_struct *png, const char *msg) {
    const State &st = *static_cast<State *>(png_get_error_ptr(png));
    Warning("Writing %s: LibPNG: %s", st.path.c_str(), msg);
}

void WriteData(png_struct *png, png_byte *data, png_size_t length) {
    const State &st = *static_cast<State *>(png_get_io_ptr(png));
    size_t pos = 0;
    while (pos < length) {
        ssize_t r = write(st.fdes, data + pos, length - pos);
        if (r == -1) {
            int ecode = errno;
            ErrorErrno(ecode, "Could not write %s", st.path.c_str());
            longjmp(png_jmpbuf(png), 1);
        }
        pos += r;
    }
}

void OutputFlush(png_struct *png) {
    (void)png;
}

} // namespace

bool WritePNG(const std::string &path, const void *data, int width,
              int height) {
    State st{path};
    st.png = png_create_write_struct(PNG_LIBPNG_VER_STRING, &st, UserError,
                                     UserWarning);
    if (st.png == nullptr) {
        Error("Could not write %s: could not initialize LibPNG", path.c_str());
        return false;
    }
    if (setjmp(png_jmpbuf(st.png)) != 0) {
        return false;
    }
    st.info = png_create_info_struct(st.png);
    if (st.info == nullptr) {
        Error("Could not write %s: could not create LibPNG info structure",
              path.c_str());
        return false;
    }
    if (!st.Open()) {
        return false;
    }
    png_set_write_fn(st.png, &st, WriteData, OutputFlush);
    png_set_IHDR(st.png, st.info, width, height, 8, PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    png_write_info(st.png, st.info);
    png_set_filler(st.png, 0, PNG_FILLER_BEFORE);
    st.rows.resize(height);
    for (int i = 0; i < height; i++) {
        st.rows[i] = const_cast<unsigned char *>(
            static_cast<const unsigned char *>(data) + i * width * 4);
    }
    png_write_image(st.png, st.rows.data());
    png_write_end(st.png, nullptr);
    return st.Commit();
}

} // namespace tcm

#endif /* !__APPLE__ */
