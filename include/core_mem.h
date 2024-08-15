#pragma once

#include <core_API.h>
#include <core_types.h>

namespace core {

using namespace coretypes;

CORE_API_EXPORT void* memcopy(void* dest, const void* src, addr_size len);
CORE_API_EXPORT void* memset(void* dest, u8 c, addr_size n);
CORE_API_EXPORT i32 memcmp(const void* s1, const void* s2, addr_size n);

/**
 * @brief Fills a memory region with the given value.
 *
 * @param dest The memory region to fill.
 * @param dstLen The length of the memory region.
 * @param val The value to fill the memory region with.
*/
template <typename T>
void memfill(void* dest, addr_size dstLen, const T& val) {
    u8* p = reinterpret_cast<u8*>(dest);
    const u8* vbytes = reinterpret_cast<const u8*>(&val);
    addr_size vsize = sizeof(val);
    for (addr_size i = 0; i < dstLen * vsize; i+=vsize) {
        core::memcopy(p + i, vbytes, vsize);
    }
}

/**
 * @brief Aligns an integer to the next multiple of the given alignment.
 *        For example, if the machine is 64bit then Align(5) will return 8.
 *
 * @note Aligning { n : n > (addr_size maximum) - 8 } will wrap around to 0.
 *
 * @param value The value to align.
 * @return T`he aligned value.
*/
constexpr addr_size align(addr_size n) {
    return (n + sizeof(addr_size) - 1) & ~(sizeof(addr_size) - 1);
}

/**
 * @brief Appends a value to a pointer and advances the pointer by the size of the value.
 *        Does NOT allocate memory and does not check for overflows!
 *
 * @param dst The pointer to append to.
 * @param val The value to append.
 * @return The pointer after the value was appended.
*/
template <typename T>
constexpr T* append(T* dst, const T& val) {
    *dst = val;
    return dst + 1;
}

/**
 * @brief Removes a value from a pointer at a given index and shifts all values after it.
 *        Does NOT check for overflows!
 *
 * @param dst The pointer to remove from.
 * @param idx The index to remove at.
 * @param len The length of the pointer.
*/
template <typename T>
constexpr T* removeAt(T* dst, addr_size idx, addr_size len) {
    if (idx >= len) return dst;
    if (idx == len - 1) return dst + len - 1;
    for (addr_size i = idx; i < len - 1; ++i) {
        dst[i] = dst[i + 1];
    }
    return dst;
}

/**
 * @brief Generic swap function that creates a temporary copy.
 *
 * @param a
 * @param b
*/
template <typename T>
constexpr void swap(T& a, T& b) {
    T tmp = a;
    a = b;
    b = tmp;
}

/**
 * @brief Swaps all bytes in a and b until the given size is reached. Can't check for overflows.
 *
 * @param a
 * @param b
 * @param size
*/
CORE_API_EXPORT void swapBytes(void* a, void* b, addr_size size);

/**
 * @brief Compares two pointers and returns the difference between them.
 *
 * @param a
 * @param b
 * @return The difference between the two pointers.
*/
inline addr_size ptrDiff(const void* a, const void* b) {
    return reinterpret_cast<addr_size>(a) - reinterpret_cast<addr_size>(b);
}

/**
 * @brief Advances a pointer by the given offset.
 *
 * @param ptr
 * @param off The offset is in bytes.
 * @return The advanced pointer.
*/
inline void* ptrAdvance(void* ptr, addr_size off) {
    return reinterpret_cast<void*>(reinterpret_cast<char*>(ptr) + off);
}

} // namespace core
