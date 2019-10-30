#include "dev/callback.hpp"

namespace tcm {

void CallbackList::Call() {
    for (size_t i = 0; i < list_.size(); i++) {
        list_[i]();
    }
}

namespace {

CallbackList persistent;
CallbackList transient;

} // namespace

void Schedule(Callback cb) {
    transient.Add(std::move(cb));
}

void ScheduleEveryFrame(Callback cb) {
    persistent.Add(std::move(cb));
}

void InvokeCallbacks() {
    persistent.Call();
    transient.Call();
    transient.Clear();
}

} // namespace tcm
