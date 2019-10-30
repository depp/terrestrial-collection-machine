// path.cpp - Path utilities.
#include "dev/path.hpp"

#include "dev/log.hpp"

#include <algorithm>
#include <charconv>

#include <dirent.h>
#include <sys/stat.h>

namespace tcm {

PathTemplate::PathTemplate(std::string dir, std::string prefix,
                           std::string suffix)
    : did_init_{false},
      dir_{std::move(dir)},
      prefix_{std::move(prefix)},
      suffix_{std::move(suffix)},
      counter_{0} {}

std::string PathTemplate::Create() {
    if (!did_init_) {
        Init();
    }
    std::string r{dir_};
    r.push_back('/');
    r.append(prefix_);
    std::string ctr = std::to_string(counter_);
    for (size_t i = ctr.size(); i < 4; i++) {
        r.push_back('0');
    }
    r.append(ctr);
    r.append(suffix_);
    counter_++;
    return r;
}

void PathTemplate::Init() {
    did_init_ = true;
    DIR *dir = opendir(dir_.c_str());
    if (dir == nullptr) {
        int ecode = errno;
        if (ecode == ENOENT) {
            int r = mkdir(dir_.c_str(), 0777);
            if (r != 0) {
                ErrorErrno(errno, "Could not create %s", dir_.c_str());
            }
            return;
        }
        ErrorErrno(ecode, "Could not list %s", dir_.c_str());
        return;
    }
    std::string name;
    unsigned long next = 0;
    while (true) {
        errno = 0;
        dirent *ent = readdir(dir);
        if (ent == nullptr) {
            break;
        }
        name.assign(ent->d_name);
        if (name.size() <= prefix_.size() ||
            name.compare(0, prefix_.size(), prefix_) != 0) {
            continue;
        }
        unsigned long value;
        auto result = std::from_chars(name.data() + prefix_.size(),
                                      name.data() + name.size(), value);
        if (result.ptr == name.data() + prefix_.size()) {
            continue;
        }
        next = std::max(next, value + 1);
    }
    int ecode = errno;
    if (ecode != 0) {
        ErrorErrno(ecode, "Could not list %s", dir_.c_str());
    }
    counter_ = next;
}

} // namespace tcm
