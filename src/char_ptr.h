#pragma once

#include "API.h"
#include "types.h"
#include "utils.h"
#include "math.h"
#include "ints.h"

namespace core
{

using namespace coretypes;

template<typename UInt>
CORE_API_EXPORT UInt DigitToCh(UInt digit) {  return (digit % 10) + '0'; }

template<typename TInt>
CORE_API_EXPORT void IntToCptr(TInt n, char* out) {
    static_assert(sizeof(n) <= 8, "Invalid TIint paramater.");
    Assert(out != nullptr);
    if (n < 0) {
        *out++ = '-';
        n = -n;
    }
    i32 dc = DigitCount(n);
    for (i32 i = dc - 1; i >= 0; i--) {
        i32 curr = (n / TInt(PowI64(10, i))) % 10;
        *out++ = DigitToCh(curr);
        dc--;
    }
}

CORE_API_EXPORT ptr_size CptrLen(const char* p);
CORE_API_EXPORT i32      CptrCmp(const char* a, const char* b);
CORE_API_EXPORT i32      CptrCmp(const uchar* a, const uchar* b);
CORE_API_EXPORT void     CptrCpy(const char *src, ptr_size n, char *dst);
CORE_API_EXPORT u64      CptrIndexOf(const char *src, ptr_size srcLen, const char *val, ptr_size valLen);
CORE_API_EXPORT u64      CptrIndexOfCh(const char *src, ptr_size srcLen, char val);

} // namespace core
