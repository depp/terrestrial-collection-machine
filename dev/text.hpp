// text.hpp - Text rendering.
#pragma once

#include <string>

namespace tcm {

void TextInit();
void TextDraw();

// A status item which can be displayed on the screen.
class StatusItem {
public:
    explicit StatusItem(std::string name);
    StatusItem(const StatusItem &) = delete;
    ~StatusItem();
    StatusItem &operator=(const StatusItem &) = delete;

    const std::string &name() const { return name_; }
    const std::string &value() const { return text_; }

    void Clear();
    void Set(std::string s);

private:
    std::string name_;
    std::string text_;
};

} // namespace tcm
