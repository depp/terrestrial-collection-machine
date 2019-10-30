// image.cpp - Image I/O.
#include "dev/image.hpp"

#include "dev/log.hpp"

#if defined __APPLE__

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <ApplicationServices/ApplicationServices.h>

namespace tcm {

namespace {

void FreeData(void *info, const void *data, size_t size) {
    (void)data;
    (void)size;
    (void)info;
}

struct State {
    State() = default;
    State(const State &) = delete;
    State &operator=(const State &) = delete;
    ~State();

    std::string path;
    int fdes = -1;
    CGColorSpaceRef color_space = nullptr;
    CGDataProviderRef data = nullptr;
    CGImageRef image = nullptr;
    CGDataConsumerRef consumer = nullptr;
    CGImageDestinationRef dest = nullptr;
};

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
    if (fdes != -1) {
        close(fdes);
        unlink(path.c_str());
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
    State st;
    st.path = path;
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
    st.fdes = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (st.fdes == -1) {
        ErrorErrno(errno, "Could not create %s", path.c_str());
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

    int r = close(st.fdes);
    st.fdes = -1;
    if (r != 0) {
        int ecode = errno;
        unlink(path.c_str());
        ErrorErrno(ecode, "Could not write to %s", path.c_str());
        return false;
    }
    return true;
}

} // namespace tcm

#endif
