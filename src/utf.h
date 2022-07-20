#pragma once

#include "API.h"
#include "types.h"
#include "tuple.h"

namespace core
{

using namespace coretypes;

CORE_API_EXPORT bool IsValidUTF8Encoding(const uchar* utf, u32 len);
CORE_API_EXPORT core::Tuple<rune, bool> RuneFromBytes(const uchar* utf, u32 len);
CORE_API_EXPORT core::Tuple<u32, bool> RuneToBytes(const rune rune, uchar* utf);

} // namespace core
