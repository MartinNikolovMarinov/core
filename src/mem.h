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

constexpr void swap(void* a, void* b, ptr_size size) {
  // TODO: Use SIMD probably.
  u8* a_ = (u8*)a;
  u8* b_ = (u8*)b;
  for (ptr_size i = 0; i < size; ++i) {
    u8 tmp = a_[i];
    a_[i] = b_[i];
    b_[i] = tmp;
  }
}

} // namespace core
