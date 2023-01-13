#pragma once

#include <API.h>
#include <types.h>
#include <expected.h>

namespace core {

using namespace coretypes;

CORE_API_EXPORT bool is_valid_utf8_encoding(const uchar* utf, u32 len);

CORE_API_EXPORT core::expected<rune, bool> rune_from_bytes(const uchar* utf, u32 len);
CORE_API_EXPORT rune                       rune_from_bytes_skip_check(const uchar* utf, u32 len);
CORE_API_EXPORT u32                        rune_to_bytes(const rune rune, uchar* utf);

} // namespace core
