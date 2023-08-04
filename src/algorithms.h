#pragma once

#include "types.h"
#include "arr.h"
#include "static_arr.h"

namespace core {

using namespace coretypes;

/**
 * \brief Finds the index of the first element in the array that satisfies the predicate. The compiler should be smart
 *        enough to completely inline this function, but if that is untrue, consider using somthing else.
 *
 *        Example predicate:
 *          [](auto& elem, ptr_size i) { return elem == i; }
 *          [closureVar](i32 elem, ptr_size) { return elem == closureVar; }
 *
 * \param arr The array to search.
 * \param pred The predicate to satisfy.
 * \return The index of the first element that satisfies the predicate, or -1 if no element satisfies the predicate.
*/
template <typename T, typename TAllocator, typename TPredicate>
inline ptr_size find(const core::arr<T, TAllocator>& arr, TPredicate pred) {
    for (ptr_size i = 0; i < arr.len(); ++i) {
        if (pred(arr[i], i)) return i;
    }
    return -1;
}

/**
 * \brief Finds the index of the first element in the array that satisfies the predicate. The compiler should be smart
 *        enough to completely inline this function, but if that is untrue, consider using somthing else.
 *
 *        Example predicate:
 *          [](auto& elem, ptr_size i) { return elem == i; }
 *          [closureVar](i32 elem, ptr_size) { return elem == closureVar; }
 *
 * \param arr The static array to search.
 * \param pred The predicate to satisfy.
 * \return The index of the first element that satisfies the predicate, or -1 if no element satisfies the predicate.
*/
template <typename T, core::ptr_size N, typename TPredicate>
inline constexpr ptr_size find(const core::sarr<T, N>& arr, TPredicate pred) {
    for (ptr_size i = 0; i < arr.len(); ++i) {
        if (pred(arr[i], i)) return i;
    }
    return -1;
}

} // namespace core
