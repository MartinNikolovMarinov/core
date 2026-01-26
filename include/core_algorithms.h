#pragma once

#include <core_arr.h>
#include <core_hash_map.h>
#include <core_traits.h>
#include <core_types.h>

namespace core {

using namespace coretypes;

template <typename T, typename TPredicate>                      inline constexpr addr_off find(const T* arr, addr_size len, TPredicate pred);
template <typename T, AllocatorId AllocId, typename TPredicate> inline addr_off           find(const ArrList<T, AllocId>& arr, TPredicate pred);
template <typename T, addr_size N, typename TPredicate>         inline constexpr addr_off find(const ArrStatic<T,  N>& arr, TPredicate pred);
template <typename T, typename TPredicate>                      inline constexpr addr_off find(const Memory<T>& memory, TPredicate pred);

template <typename T, typename TEq>                           inline constexpr void pushUnique(T* arr, addr_size len, const T& el, TEq eqFn);
template <typename T, typename TEq>                           inline constexpr void pushUnique(T* arr, addr_size len, T&& el, TEq eqFn);
template <typename T, AllocatorId AllocId, typename TEq>      inline void           pushUnique(ArrList<T, AllocId>& arr, const T& el, TEq eqFn);
template <typename T, AllocatorId AllocId, typename TEq>      inline void           pushUnique(ArrList<T, AllocId>& arr, T&& el, TEq eqFn);
template <typename T, addr_size N, typename TEq>              inline constexpr void pushUnique(ArrStatic<T, N>& arr, const T& el, TEq eqFn);
template <typename T, addr_size N, typename TEq>              inline constexpr void pushUnique(ArrStatic<T, N>& arr, T&& el, TEq eqFn);

template <typename T, typename TPredicate>                                         constexpr inline bool forAll(const T* arr, addr_size len, TPredicate pred);
template <typename T, AllocatorId AllocId, typename TPredicate>                    inline bool           forAll(const core::ArrList<T, AllocId>& arr, TPredicate pred);
template <typename T, addr_size N, typename TPredicate>                            constexpr inline bool forAll(const core::ArrStatic<T, N>& arr, TPredicate pred);
template <typename TKey, typename TVal, AllocatorId AllocId, typename TPredicate>  inline bool           forAll(const core::HashMap<TKey, TVal, AllocId>& a,
                                                                                                                const core::HashMap<TKey, TVal, AllocId>& b,
                                                                                                                TPredicate pred);

template <typename T, typename TCompare>   inline constexpr void     quickSort(T* arr, addr_size len, TCompare compare);
template <typename T, typename TPredicate> inline constexpr addr_off binarySearch(T* arr, addr_size len, TPredicate pred);

template <typename T, typename TPredicate> inline constexpr bool compareArraysBy(T* a, T* b, addr_size len, TPredicate pred);

// Find element in raw pointer.
template <typename T, typename TPredicate>
inline constexpr addr_off find(const T* arr, addr_size len, TPredicate pred) {
    for (addr_off i = 0; i < addr_off(len); ++i) {
        auto& v = arr[addr_size(i)];
        if (pred(v, addr_size(i))) return i;
    }
    return -1;
}
// Find element in ArrList.
template <typename T, AllocatorId AllocId, typename TPredicate>
inline addr_off find(const ArrList<T, AllocId>& arr, TPredicate pred) {
    return find(arr.data(), arr.len(), pred);
}
// Find element in ArrStatic.
template <typename T, addr_size N, typename TPredicate>
inline constexpr addr_off find(const ArrStatic<T, N>& arr, TPredicate pred) {
    return find(arr.data(), arr.len(), pred);
}
// Find element in Memory.
template <typename T, typename TPredicate>
inline constexpr addr_off find(const Memory<T>& memory, TPredicate pred) {
    return find(memory.data(), memory.len(), pred);
}

namespace detail {

template <typename TArrContainer, typename TElement, typename TEq>
inline constexpr void pushUniqueFreeTmpl(TArrContainer arr, TElement el, TEq eqFn) {
    static_assert(std::is_reference_v<TArrContainer>, "TArrContainer must be a reference type.");
    static_assert(std::is_reference_v<TElement>, "TElement must be a reference type.");

    for (addr_size i = 0; i < arr.len(); ++i) {
        auto& v = arr[i];
        if (eqFn(v, i, el)) return;
    }

    if (std::is_rvalue_reference_v<decltype(el)>) {
        arr.push(std::move(el));
    }
    else {
        arr.push(el);
    }
}

} // core

// Append unique element to raw pointer.
template <typename T, typename TEq>
inline constexpr void pushUnique(T* arr, addr_size len, const T& el, TEq eqFn) {
    for (addr_size i = 0; i < len; ++i) {
        auto& v = arr[i];
        if (eqFn(v, i, el)) return;
    }
}
template <typename T, typename TEq>
inline constexpr void pushUnique(T* arr, addr_size len, T&& el, TEq eqFn) {
    for (addr_size i = 0; i < len; ++i) {
        auto& v = arr[i];
        if (eqFn(v, i, el)) return;
    }
    core::append(arr, len, std::move(el));
}

// Append unique element to ArrList.
template <typename T, AllocatorId AllocId, typename TEq>
inline void pushUnique(ArrList<T, AllocId>& arr, const T& el, TEq eqFn) {
    using T1 = ArrList<T, AllocId>&;
    using T2 = const T&;
    using T3 = TEq;
    detail::pushUniqueFreeTmpl<T1, T2, T3>(arr, el, eqFn);
}
template <typename T, AllocatorId AllocId, typename TEq>
inline void pushUnique(ArrList<T, AllocId>& arr, T&& el, TEq eqFn) {
    using T1 = ArrList<T, AllocId>&;
    using T2 = T&&;
    using T3 = TEq;
    detail::pushUniqueFreeTmpl<T1, T2, T3>(arr, std::move(el), eqFn);
}

// Append unique element to ArrStatic.
template <typename T, addr_size N, typename TEq>
inline constexpr void pushUnique(ArrStatic<T, N>& arr, const T& el, TEq eqFn) {
    using T1 = ArrStatic<T, N>&;
    using T2 = const T&;
    using T3 = TEq;
    detail::pushUniqueFreeTmpl<T1, T2, T3>(arr, el, eqFn);
}
template <typename T, addr_size N, typename TEq>
inline constexpr void pushUnique(ArrStatic<T, N>& arr, T&& el, TEq eqFn) {
    using T1 = ArrStatic<T, N>&;
    using T2 = T&&;
    using T3 = TEq;
    detail::pushUniqueFreeTmpl<T1, T2, T3>(arr, std::move(el), eqFn);
}

// Call predicate for each element in raw pointer.
template <typename T, typename TPredicate>
inline constexpr bool forAll(const T* arr, addr_size len, TPredicate pred) {
    for (addr_size i = 0; i < len; ++i) {
        if (!pred(arr[i], i)) {
            return false;
        }
    }
    return true;
}
// Call predicate for each element in ArrList.
template <typename T, AllocatorId AllocId, typename TPredicate>
inline bool forAll(const core::ArrList<T, AllocId>& arr, TPredicate pred) {
    return forAll(arr.data(), arr.len(), pred);
}
// Call predicate for each element in ArrStatic.
template <typename T, addr_size N, typename TPredicate>
inline constexpr bool forAll(const core::ArrStatic<T, N>& arr, TPredicate pred) {
    return forAll(arr.data(), arr.len(), pred);
}
// Call predicate for each element in HashMap.
template <typename TKey, typename TVal, AllocatorId AllocId, typename TPredicate>
inline bool forAll(const core::HashMap<TKey, TVal, AllocId>& a,
                   const core::HashMap<TKey, TVal, AllocId>& b,
                   TPredicate pred) {
    bool ret = true;
    a.entries([&](const auto& key, const auto& val) -> bool {
        if (!pred(key, val, b)) {
            ret = false;
            return false;
        }
        return true;
    });
    return ret;
}

namespace detail {

template <typename T, typename TCompare>
constexpr  addr_size partitionHoareHalfOpen(T* arr, addr_size lo, addr_size hi, TCompare compare)
{
    // partitions [lo, hi) and returns split point p such that:
    // [lo, p) <= pivot and [p, hi) >= pivot (conceptually)

    // median-of-three on lo, mid, hi-1
    addr_size last = hi - 1;
    addr_size mid  = lo + (hi - lo) / 2;

    if (compare(arr[mid],  arr[lo])  < 0) core::swap(arr[mid],  arr[lo]);
    if (compare(arr[last], arr[lo])  < 0) core::swap(arr[last], arr[lo]);
    if (compare(arr[last], arr[mid]) < 0) core::swap(arr[last], arr[mid]);

    T pivot = arr[mid]; // copy: pivot must not change

    addr_size i = lo;
    addr_size j = hi;

    for (;;) {
        while (i < hi && compare(arr[i], pivot) < 0) ++i;

        // j is in (lo, hi]; decrement first, but keep it bounded
        do { --j; } while (j > lo && compare(arr[j], pivot) > 0);

        if (i >= j) return i; // split point (half-open friendly)

        core::swap(arr[i], arr[j]);
        ++i;
    }
}

template <typename T, typename TCompare>
inline constexpr void quickSortImpl(T* arr, addr_size lo, addr_size hi, TCompare compare)
{
    while (hi - lo > 1) {
        addr_size p = partitionHoareHalfOpen(arr, lo, hi, compare);

        if (p - lo < hi - p) {
            quickSortImpl(arr, lo, p, compare);
            lo = p;
        }
        else {
            quickSortImpl(arr, p, hi, compare);
            hi = p;
        }
    }
}

} // namespace detail

template <typename T, typename TCompare>
inline constexpr void quickSort(T* arr, addr_size len, TCompare compare)
{
    if (len < 2) return;
    detail::quickSortImpl(arr, 0, len, compare);
}

namespace detail {

template <typename T, typename TCompare>
inline constexpr addr_off binarySearchImpl(T* arr, addr_off low, addr_off hi, TCompare compare) {
    while (low <= hi) {
        addr_off m = low + (hi - low) / 2;

        auto result = compare(arr[m], addr_size(m));
        if (result == 0) return addr_off(m);

        if (result > 0) {
            if (m == 0) break; // avoid unsigned underflow
            hi = m - 1;
        }
        else {
            low = m + 1;
        }
    }
    return addr_off(-1);
}

} // namespace detail

template <typename T, typename TCompare>
inline constexpr addr_off binarySearch(T* arr, addr_size len, TCompare compare) {
    return detail::binarySearchImpl(arr, 0, addr_off(len) - 1, compare);
}

template <typename T, typename TPredicate>
inline constexpr bool compareArraysBy(T* a, T* b, addr_size len, TPredicate pred) {
    for (addr_size i = 0; i < len; i++) {
        if (!pred(a[i], b[i])) {
            return false;
        }
    }
    return true;
}

} // namespace core
