#pragma once

#include <core_types.h>
#include <core_arr.h>

namespace core {

using namespace coretypes;

/**
 * \brief Finds the index of the first element in the array that satisfies the predicate.
 *
 *        Example predicate:
 *          [](auto& elem, addr_off i) { return elem == i; }
 *          [closureVar](i32 elem, addr_off) { return elem == closureVar; }
 *
 * \param arr The array to search.
 * \param pred The predicate to satisfy.
 * \return The index of the first element that satisfies the predicate, or -1 if no element satisfies the predicate.
*/
template <typename T, typename TPredicate>
inline addr_off find(const ArrList<T>& arr, TPredicate pred) {
    for (addr_off i = 0; i < addr_off(arr.len()); ++i) {
        auto& v = arr[addr_size(i)];
        if (pred(v, i)) return i;
    }
    return -1;
}
template <typename T, addr_size N, typename TPredicate>
inline constexpr addr_off find(const ArrStatic<T, N>& arr, TPredicate pred) {
    for (addr_off i = 0; i < addr_off(arr.len()); ++i) {
        auto& v = arr[addr_size(i)];
        if (pred(v, i)) return i;
    }
    return -1;
}
template <typename T, typename TPredicate>
inline addr_off find(const T* arr, addr_off len, TPredicate pred) {
    for (addr_off i = 0; i < len; ++i) {
        auto& v = arr[addr_size(i)];
        if (pred(v, i)) return i;
    }
    return -1;
}

/**
 * \brief Pushes an element to the array if it is not already present, this is determined by the equality function.
 *
 *       Example equality function:
 *          [closureVar](auto& curr) -> bool { return curr == closureVar; }
 *
 * \param arr The array to push to.
 * \param el The element to push.
 * \param eqFn The equality function.
*/
template <typename T, typename TEq>
inline void pushUnique(ArrList<T>& arr, const T& el, TEq eqFn) {
    for (addr_off i = 0; i < addr_off(arr.len()); ++i) {
        auto& v = arr[addr_size(i)];
        if (eqFn(v, i, el)) return;
    }
    arr.push(el);
}
template <typename T, typename TEq>
inline void pushUnique(ArrList<T>& arr, T&& el, TEq eqFn) {
    for (addr_off i = 0; i < addr_off(arr.len()); ++i) {
        auto& v = arr[addr_size(i)];
        if (eqFn(v, i, el)) return;
    }
    arr.push(el);
}
template <typename T, addr_size N, typename TPredicate>
inline constexpr void pushUnique(ArrStatic<T, N>& arr, const T& el, TPredicate eqFn) {
    for (addr_off i = 0; i < addr_off(arr.len()); ++i) {
        auto& v = arr[addr_size(i)];
        if (eqFn(v, i, el)) return;
    }
    arr.push(el);
}
template <typename T, addr_size N, typename TPredicate>
inline constexpr void pushUnique(ArrStatic<T, N>& arr, T&& el, TPredicate eqFn) {
    for (addr_off i = 0; i < addr_off(arr.len()); ++i) {
        auto& v = arr[addr_size(i)];
        if (eqFn(v, i, el)) return;
    }
    arr.push(el);
}

} // namespace core
