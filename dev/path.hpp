// path.hpp - Path utilities.
#pragma once

#include <string>

namespace tcm {

// A template for generating unique paths.
class PathTemplate {
public:
    PathTemplate(std::string dir, std::string prefix, std::string suffix);
    PathTemplate(const PathTemplate &) = delete;
    PathTemplate &operator=(const PathTemplate &) = delete;

    // Create a unique path using the template.
    std::string Create();

private:
    void Init();

    bool did_init_;
    std::string dir_;
    std::string prefix_;
    std::string suffix_;
    unsigned long counter_;
};

} // namespace tcm
