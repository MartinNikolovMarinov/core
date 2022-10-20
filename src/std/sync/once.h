#pragma once

#include <API.h>

#include <types.h>

#include <atomic>
#include <mutex>

namespace core::sync {

struct CORE_API_EXPORT Once {
    template<typename TFunc>
    void Do(TFunc fn) {
        if (done.load() == false) {
            // Take slow path:
            std::lock_guard<std::mutex> lock{mux};
            if (done.load() == false) {
                fn();
                done.store(true);
            }
        }
    }

private:
    std::atomic<bool> done = false;
    std::mutex mux;
};

} // namespace sync
