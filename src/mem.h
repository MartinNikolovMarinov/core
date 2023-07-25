#pragma once

#include <API.h>
#include <types.h>

namespace core {

using namespace coretypes;

CORE_API_EXPORT void memcopy(void* dest, const void* src, ptr_size len);
CORE_API_EXPORT void* memset(void* dest, i32 c, ptr_size n);
CORE_API_EXPORT i32 memcmp(const void* s1, const void* s2, ptr_size n);

/**
 * @brief Aligns an integer to the next multiple of the given alignment.
 *        For example, if the machine is 64bit then Align(5) will return 8.
 *
 * @param value The value to align.
 * @return The aligned value.
*/
constexpr ptr_size align(ptr_size n) {
    return (n + sizeof(ptr_size) - 1) & ~(sizeof(ptr_size) - 1);
}

CORE_API_EXPORT void swap_bytes(void* a, void* b, ptr_size size);

template <typename T>
constexpr void swap(T& a, T& b) {
    T tmp = a;
    a = b;
    b = tmp;
}

} // namespace core
