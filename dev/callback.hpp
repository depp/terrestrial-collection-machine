#pragma once

#include <functional>
#include <vector>

namespace tcm {

// A callback function.
using Callback = std::function<void()>;

// A list of callback functions.
class CallbackList {
public:
    // Add a callback to the list.
    void Add(Callback cb) { list_.emplace_back(std::move(cb)); }
    // Call all callbacks in the list.
    void Call();
    // Remove all callbacks from the list.
    void Clear() { list_.clear(); }

private:
    std::vector<Callback> list_;
};

// Schedule a callback to be called this frame.
void Schedule(Callback cb);

// Schedule a callback to be called every frame.
void ScheduleEveryFrame(Callback cb);

// Invoke all scheduled callbacks.
void InvokeCallbacks();

} // namespace tcm
