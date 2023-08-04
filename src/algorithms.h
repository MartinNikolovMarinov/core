#pragma once

#include "types.h"
#include "arr.h"
#include "static_arr.h"

namespace core {

using namespace coretypes;

/**
 * \brief Finds the index of the first element in the array that satisfies the predicate.
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
template <typename T, core::ptr_size N, typename TPredicate>
inline constexpr ptr_size find(const core::sarr<T, N>& arr, TPredicate pred) {
    for (ptr_size i = 0; i < arr.len(); ++i) {
        if (pred(arr[i], i)) return i;
    }
    return -1;
}

/**
 * \brief Appends an element to the array if it is not already present, this is determined by the equality function.
 *
 *       Example equality function:
 *          [closureVar](auto& curr) -> bool { return curr == closureVar; }
 *
 * \param arr The array to append to.
 * \param el The element to append.
 * \param eqFn The equality function.
*/
template <typename T, typename TAllocator, typename TEq>
inline void appendUnique(core::arr<T, TAllocator>& arr, const T& el, TEq eqFn) {
    for (ptr_size i = 0; i < arr.len(); ++i) {
        if (eqFn(arr[i])) return;
    }
    arr.append(el);
}
template <typename T, typename TAllocator, typename TEq>
inline void appendUnique(core::arr<T, TAllocator>& arr, T&& el, TEq eqFn) {
    for (ptr_size i = 0; i < arr.len(); ++i) {
        if (eqFn(arr[i])) return;
    }
    arr.append(el);
}
template <typename T, core::ptr_size N, typename TPredicate>
inline constexpr void appendUnique(core::sarr<T, N>& arr, const T& el, TPredicate pred) {
    for (ptr_size i = 0; i < arr.len(); ++i) {
        if (eqFn(arr[i])) return;
    }
    arr.append(el);
}
template <typename T, core::ptr_size N, typename TPredicate>
inline constexpr void appendUnique(core::sarr<T, N>& arr, T&& el, TPredicate pred) {
    for (ptr_size i = 0; i < arr.len(); ++i) {
        if (eqFn(arr[i])) return;
    }
    arr.append(el);
}

} // namespace core
