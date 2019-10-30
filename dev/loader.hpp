#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace tcm {

// Change the current directory to the Bazel workspace root.
void ChdirWorkspaceRoot(void);

// A buffer containing the contents of a file.
using DataBuffer = std::shared_ptr<const std::vector<char>>;

// Callback for when a file changes. The function is passed the new contents of
// the file, or nullptr if the file does not exist or could not be read.
using WatchCallback = std::function<void(const DataBuffer &)>;

// Watch for changes in the given file, and call a function when it changes.
void WatchFile(std::string path, WatchCallback callback);

} // namespace tcm
