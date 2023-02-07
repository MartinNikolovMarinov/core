#pragma once

#include <API.h>
#include <types.h>
#include <utils.h>
#include <core_math.h>
#include <ints.h>

namespace core
{

using namespace coretypes;

template<typename UInt>
CORE_API_EXPORT char digit_to_char(UInt digit) {  return (digit % 10) + '0'; }

template<typename TInt>
CORE_API_EXPORT void int_to_cptr(TInt n, char* out) {
    static_assert(sizeof(n) <= 8, "Invalid TIint paramater.");
    Assert(out != nullptr);
    if (n < 0) {
        *out++ = '-';
        n = -n;
    }
    i32 dc = digit_count(n);
    for (i32 i = dc - 1; i >= 0; i--) {
        i32 curr = (n / TInt(pow_u64(10, i))) % 10;
        *out++ = digit_to_char(curr);
        dc--;
    }
}

CORE_API_EXPORT ptr_size cptr_len(const char* p);
CORE_API_EXPORT i32      cptr_cmp(const char* a, const char* b);
CORE_API_EXPORT i32      cptr_cmp(const uchar* a, const uchar* b);
CORE_API_EXPORT void     cptr_cpy(const char *src, ptr_size n, char *dst);
CORE_API_EXPORT u64      cptr_idx_of(const char *src, ptr_size srcLen, const char *val, ptr_size valLen);
CORE_API_EXPORT u64      cptr_idx_of_char(const char *src, ptr_size srcLen, char val);

} // namespace core
