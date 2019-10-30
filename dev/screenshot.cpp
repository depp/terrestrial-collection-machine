// screenshot.cpp - Record the contents of the framebuffer.
#include "dev/screenshot.hpp"

#include "dev/callback.hpp"
#include "dev/image.hpp"
#include "dev/log.hpp"
#include "tcm/gl.h"

#include <memory>
#include <string>

namespace tcm {

namespace {

// Next counter value to use for screenshot paths.
int screenshot_counter;

// Create a unique path for a screenshot.
std::string CreateScreenshotPath() {
    std::string ctr = std::to_string(screenshot_counter);
    screenshot_counter++;
    std::string path{"shot"};
    for (size_t i = ctr.size(); i < 4; i++) {
        path.push_back('0');
    }
    path.append(ctr);
    path.append(".png");
    return path;
}

// A screenshot of the buffer.
class Screenshot {
public:
    Screenshot();
    Screenshot(const Screenshot &) = delete;
    ~Screenshot();
    Screenshot &operator=(const Screenshot &) = delete;

    // Capture the framebuffer.
    void Capture();

    // Save the capture to disk.
    void Save();

private:
    // Save the capt
    void SaveData(const void *data);

    bool has_shot_;
    GLuint buffer_;
    int width_;
    int height_;
};

Screenshot::Screenshot()
    : has_shot_{false}, buffer_{0}, width_{0}, height_{0} {}

Screenshot::~Screenshot() {
    if (buffer_) {
        glDeleteBuffers(1, &buffer_);
    }
}

void Screenshot::Capture() {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    if (buffer_ == 0) {
        glGenBuffers(1, &buffer_);
        if (buffer_ == 0) {
            ErrorGL(glGetError(), "glGenBuffers");
            return;
        }
    }
    int x = viewport[0];
    int y = viewport[1];
    int width = viewport[2];
    int height = viewport[3];
    glBindBuffer(GL_PIXEL_PACK_BUFFER, buffer_);
    glBufferData(GL_PIXEL_PACK_BUFFER, width * height * 4, nullptr,
                 GL_STREAM_READ);
    glReadPixels(x, y, width, height, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, 0);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    has_shot_ = true;
    width_ = width;
    height_ = height;
}

void Screenshot::Save() {
    if (!has_shot_) {
        return;
    }
    has_shot_ = false;
    glBindBuffer(GL_PIXEL_PACK_BUFFER, buffer_);
    void *data = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
    if (data == nullptr) {
        ErrorGL(glGetError(), "glMapBuffer");
    } else {
        std::string path = CreateScreenshotPath();
        if (WritePNG(path, data, width_, height_)) {
            std::fprintf(stderr, "Wrote screenshot %s\n", path.c_str());
        }
        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    }
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

} // namespace

void CaptureScreenshot() {
    std::shared_ptr<Screenshot> shot = std::make_shared<Screenshot>();
    shot->Capture();
    // We delay here to avoid stalling the rendering pipeline.
    Schedule([shot] {
        shot->Save();
        Schedule([shot] { shot->Save(); });
    });
}

} // namespace tcm
