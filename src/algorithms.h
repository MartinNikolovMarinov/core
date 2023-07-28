#pragma once

#include "types.h"
#include "arr.h"
#include "static_arr.h"

namespace core {

using namespace coretypes;

template <typename T, typename TAllocator, typename TPredicate>
ptr_size find(const core::arr<T, TAllocator>& arr, TPredicate pred) {
    for (ptr_size i = 0; i < arr.len(); ++i) {
        if (pred(arr[i], i)) return i;
    }
    return -1;
}

template <typename T, core::ptr_size N, typename TPredicate>
constexpr ptr_size find(const core::sarr<T, N>& arr, TPredicate pred) {
    for (ptr_size i = 0; i < arr.len(); ++i) {
        if (pred(arr[i], i)) return i;
    }
    return -1;
}

} // namespace core
