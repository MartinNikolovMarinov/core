#pragma once

#include <API.h>
#include <types.h>
#include <expected.h>

namespace core
{

using namespace coretypes;

CORE_API_EXPORT bool IsValidUTF8Encoding(const uchar* utf, u32 len);
// FIXME: Replace this tuples with the expected type, when that error handling is done.
CORE_API_EXPORT core::Expected<rune, bool> RuneFromBytes(const uchar* utf, u32 len);
CORE_API_EXPORT rune                       RuneFromBytesSkipCheck(const uchar* utf, u32 len);
CORE_API_EXPORT u32                        RuneToBytes(const rune rune, uchar* utf);

} // namespace core
