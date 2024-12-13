#pragma once

#include <core_API.h>
#include <core_types.h>
#include <core_cstr.h>
#include <core_mem.h>

namespace core {

using namespace coretypes;

using StrView = core::Memory<const char>;

constexpr StrView sv()                               { return StrView{nullptr, 0}; }
constexpr StrView sv(const char* str)                { return StrView{str, core::cstrLen(str)}; }
constexpr StrView sv(const char* str, addr_size len) { return StrView{str, len}; }
constexpr StrView trimWhiteSpaceLeft(StrView s);
constexpr StrView trimWhiteSpaceRight(StrView s);
constexpr StrView trimWhiteSpace(StrView s);

constexpr StrView operator""_sv(const char* str, size_t len) {
    return StrView(str, static_cast<StrView::size_type>(len));
}

constexpr inline StrView trimWhiteSpaceLeft(StrView s) {
    const char* data = core::cstrSkipSpace(s.data());
    return StrView{data, s.len()};
}

constexpr inline StrView trimWhiteSpaceRight(StrView s) {
    if (s.data() == nullptr) return s;

    addr_off boff = addr_off(s.len());
    for (; boff >= 0; boff--) {
        if (!core::isWhiteSpace(s[addr_size(boff)])) break;
    }

    return StrView{s.data(), addr_size(boff)};
}

constexpr inline StrView trim(StrView s) {
    if (s.data() == nullptr) return s;

    StrView f = trimWhiteSpaceLeft(s);
    StrView b = trimWhiteSpaceRight(s);

    return StrView{f.data(), b.len()};
}

} // namespace core

using core::operator""_sv; // use string literal globally.
