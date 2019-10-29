#include "tcm/loader.hpp"

#include "tcm/callback.hpp"
#include "tcm/log.hpp"

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

namespace tcm {

void ChdirWorkspaceRoot(void) {
    // There are two cases to handle here. If this is run from "bazel run", then
    // the current directory is inside the Bazel build directory, which is
    // somewhere in /tmp. We find the workspace directory using an environment
    // variable that Bazel sets. If this is run directly by the user, we assume
    // that the current directory is somewhere inside the workspace and search
    // upwards for the WORKSPACE file.
    static const char ERR_WORKSPACE[] = "Could not find workspace";
    int r;
    // This environment variable is set by Bazel when it runs a program.
    const char *workspace_dir = getenv("BUILD_WORKSPACE_DIRECTORY");
    if (workspace_dir != NULL) {
        r = chdir(workspace_dir);
        if (r != 0) {
            DieErrno(errno, ERR_WORKSPACE);
        }
        return;
    }
    for (int depth = 0; depth < 10; depth++) {
        struct stat st;
        r = stat("WORKSPACE", &st);
        if (r == 0) {
            return;
        }
        int ecode = errno;
        if (ecode != ENOENT) {
            DieErrno(ecode, ERR_WORKSPACE);
        }
        r = chdir("..");
        if (r != 0) {
            DieErrno(errno, ERR_WORKSPACE);
        }
    }
    Die(ERR_WORKSPACE);
}

namespace {

timespec ModTime(const struct stat &st) {
#if defined __APPLE__
    return st.st_mtimespec;
#elif defined __linux__
    return st.st_mtim;
#endif
}

// Read the contents of a file into a vector. Returns 0 on success, or the error
// code on failure.
int ReadFile(const std::string &path, std::vector<char> *data) {
    int fdes = open(path.c_str(), O_RDONLY);
    if (fdes == -1) {
        return errno;
    }
    struct stat st;
    int r = fstat(fdes, &st);
    if (r == -1) {
        int ecode = errno;
        close(fdes);
        return ecode;
    }
    size_t size = st.st_size;
    data->resize(size);
    char *ptr = data->data();
    size_t pos = 0;
    while (pos < size) {
        ssize_t amt = read(fdes, ptr + pos, size - pos);
        if (amt == -1) {
            int ecode = errno;
            close(fdes);
            return ecode;
        } else if (amt == 0) {
            data->resize(size);
            break;
        }
        pos += amt;
    }
    close(fdes);
    return 0;
}

// Return true if two timespec are equal.
bool TimeEqual(const timespec &x, const timespec &y) {
    return x.tv_sec == y.tv_sec && x.tv_nsec == y.tv_nsec;
}

// A pending file change to report.
struct Change {
    WatchCallback *callback;
    DataBuffer data;
};

// A file being watched, and the associated callback.
class Watch {
public:
    Watch(std::string path, WatchCallback callback);

    // Get any pending changes and add them to the vector.
    void GetChange(std::vector<Change> *changes);

    // Poll the file for changes.
    void Poll();

private:
    struct Contents {
        static Contents Error(int error) { return {nullptr, {0, 0}, error}; }

        DataBuffer data;
        timespec mtime;
        int error;
    };

    void SetError(int ecode);

    std::string path_;
    WatchCallback callback_;
    bool changed_;
    Contents contents_;
};

Watch::Watch(std::string path, WatchCallback callback)
    : path_{std::move(path)}, callback_{std::move(callback)} {}

void Watch::GetChange(std::vector<Change> *changes) {
    if (changed_) {
        changes->push_back({&callback_, contents_.data});
    }
    changed_ = false;
}

void Watch::Poll() {
    struct stat st;
    int r;
    r = stat(path_.c_str(), &st);
    if (r == -1) {
        SetError(errno);
        return;
    }
    timespec mtime = ModTime(st);
    if (TimeEqual(mtime, contents_.mtime)) {
        return;
    }
    std::vector<char> data;
    r = ReadFile(path_, &data);
    if (r != 0) {
        SetError(r);
        return;
    }
    if (contents_.data && data == *contents_.data) {
        return;
    }
    changed_ = true;
    contents_ = {
        std::make_shared<const std::vector<char>>(std::move(data)),
        {0, 0},
        0,
    };
}

void Watch::SetError(int ecode) {
    if (contents_.error != ecode) {
        changed_ = true;
        contents_ = {nullptr, {0, 0}, ecode};
        ErrorErrno(ecode, "%s", path_.c_str());
    }
}

std::vector<std::unique_ptr<Watch>> watches;

// Get all pending changes for watched files.
std::vector<Change> GetChanges() {
    std::vector<Change> changes;
    for (const auto &watch : watches) {
        watch->GetChange(&changes);
    }
    return changes;
}

bool PollFilesScheduled;

void PollFiles() {
    for (auto &watch : watches) {
        watch->Poll();
    }
    const std::vector<Change> changes = GetChanges();
    for (const auto &change : changes) {
        (*change.callback)(change.data);
    }
}

} // namespace

void WatchFile(std::string path,
               std::function<void(const DataBuffer &)> callback) {
    if (!PollFilesScheduled) {
        PollFilesScheduled = true;
        ScheduleEveryFrame(PollFiles);
    }
    std::unique_ptr<Watch> watch =
        std::make_unique<Watch>(std::move(path), std::move(callback));
    watches.emplace_back(std::move(watch));
}

} // namespace tcm
