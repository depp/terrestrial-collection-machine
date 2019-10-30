// image.hpp - Image I/O.
#pragma once

#include <string>

namespace tcm {

// Write a PNG image to the given path.
bool WritePNG(const std::string &path, const void *data, int width, int height);

} // namespace tcm
