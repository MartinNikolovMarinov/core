#pragma once

#include <API.h>
#include <types.h>

namespace core {

using namespace coretypes;

CORE_API_EXPORT void  memcopy(void* dest, const void* src, addr_size len);
CORE_API_EXPORT void* memset(void* dest, u8 c, addr_size n);
CORE_API_EXPORT i32   memcmp(const void* s1, const void* s2, addr_size n);

template <typename T>
void memfill(void* dest, addr_size dstLen, const T& val) {
    u8* p = reinterpret_cast<u8*>(dest);
    const u8* vbytes = reinterpret_cast<const u8*>(&val);
    addr_size vsize = sizeof(val);
    for (addr_size i = 0; i < dstLen; i+=vsize) {
        core::memcopy(p + i, vbytes, vsize);
    }
}

/**
 * @brief Aligns an integer to the next multiple of the given alignment.
 *        For example, if the machine is 64bit then Align(5) will return 8.
 *
 * @param value The value to align.
 * @return The aligned value.
*/
constexpr addr_size align(addr_size n) {
    return (n + sizeof(addr_size) - 1) & ~(sizeof(addr_size) - 1);
}

CORE_API_EXPORT void swap_bytes(void* a, void* b, addr_size size);

template <typename T>
constexpr void swap(T& a, T& b) {
    T tmp = a;
    a = b;
    b = tmp;
}

} // namespace core
