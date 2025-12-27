#pragma once

#include <core_API.h>
#include <core_types.h>
#include <core_cstr.h>
#include <core_mem.h>

namespace core {

using namespace coretypes;

using StrView = core::Memory<const char>;

constexpr        StrView sv()                               { return StrView{nullptr, 0}; }
constexpr        StrView sv(const char* str)                { return StrView{str, core::cstrLen(str)}; }
constexpr        StrView sv(const char* str, addr_size len) { return StrView{str, len}; }
          inline StrView sv(const u8* ustr, addr_size len)  { return StrView{reinterpret_cast<const char*>(ustr), len}; }
          inline StrView sv(core::Memory<u8> mem)           { return sv(mem.data(), mem.len()); }
constexpr inline StrView trimWhiteSpaceLeft(StrView s);
constexpr inline StrView trimWhiteSpaceRight(StrView s);
constexpr inline StrView trim(StrView s);
constexpr inline bool    cut(StrView s, char c, StrView& out);
constexpr inline StrView skipWhiteSpace(StrView s);
constexpr inline StrView skip(StrView s, char c);

constexpr StrView operator""_sv(const char* str, size_t len) {
    return StrView(str, static_cast<StrView::size_type>(len));
}

constexpr inline StrView trimWhiteSpaceLeft(StrView s) {
    if (s.empty()) return s;

    addr_size i = 0;
    while (i < s.len() && isWhiteSpace(s[i])) {
        i++;
    }

    addr_size len = s.len() - i;
    return StrView{&s[i], addr_size(len)};
}

constexpr inline StrView trimWhiteSpaceRight(StrView s) {
    if (s.empty()) return s;

    addr_off boff = addr_off(s.len());
    while (boff > 0 && isWhiteSpace(s[addr_size(boff - 1)])) {
        boff--;
    }

    return StrView{s.data(), addr_size(boff)};
}

constexpr inline StrView trim(StrView s) {
    if (s.empty()) return s;

    StrView leftTrimmed = trimWhiteSpaceLeft(s);
    StrView bothTrimmed = trimWhiteSpaceRight(leftTrimmed);

    return bothTrimmed;
}

constexpr inline bool cut(StrView s, char c, StrView& out) {
    out = {};

    if (s.empty()) return false;

    for (addr_size i = 0; i < s.len() - 1; i++) {
        if (s[i] == c) {
            out = { &s[i + 1], s.len() - i - 1 };
            break;
        }
    }

    return out.len() > 0;
}

constexpr inline StrView skipWhiteSpace(StrView s) {
    if (s.empty()) return s;

    addr_size i = 0;
    while (i < s.len() && isWhiteSpace(s[i])) {
        i++;
    }

    addr_size len = s.len() - i;
    return s.slice(i, len);
}

constexpr inline StrView skip(StrView s, char c) {
    if (s.empty()) return s;

    addr_size i = 0;
    while (i < s.len() && s[i] == c) {
        i++;
    }

    addr_size len = s.len() - i;
    return s.slice(i, len);
}

} // namespace core

using core::operator""_sv; // use string literal globally.
