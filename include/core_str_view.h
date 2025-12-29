#pragma once

#include <core_types.h>
#include <core_cstr.h>
#include <core_mem.h>

namespace core {

using namespace coretypes;

using StrView = core::Memory<const char>;

constexpr inline StrView sv()                               { return StrView{nullptr, 0}; }
constexpr inline StrView sv(const char* str)                { return StrView{str, core::cstrLen(str)}; }
constexpr inline StrView sv(const char* str, addr_size len) { return StrView{str, len}; }
          inline StrView sv(const u8* ustr, addr_size len)  { return StrView{reinterpret_cast<const char*>(ustr), len}; }
          inline StrView sv(core::Memory<u8> mem)           { return sv(mem.data(), mem.len()); }
constexpr inline StrView trimWhiteSpaceLeft(StrView s);
constexpr inline StrView trimWhiteSpaceRight(StrView s);
constexpr inline StrView trim(StrView s);
constexpr inline StrView cut(StrView s, char c, StrView& out);
constexpr inline StrView skipWhiteSpace(StrView s);
constexpr inline StrView skip(StrView s, char c);
constexpr inline bool startsWith(StrView s, const char* prefix);
constexpr inline bool startsWith(StrView s, StrView prefix);
constexpr inline bool endsWith(StrView s, const char* postfix);
constexpr inline bool endsWith(StrView s, StrView postfix);

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
    StrView ret = len > 0
                  ? core::sv(&s[i], addr_size(len))
                  : core::sv();
    return ret;

}

constexpr inline StrView trimWhiteSpaceRight(StrView s) {
    if (s.empty()) return core::sv();

    addr_off boff = addr_off(s.len());
    while (boff > 0 && isWhiteSpace(s[addr_size(boff - 1)])) {
        boff--;
    }

    StrView ret = boff > 0
                  ? core::sv(s.data(), addr_size(boff))
                  : core::sv();
    return ret;
}

constexpr inline StrView trim(StrView s) {
    if (s.empty()) return core::sv();

    StrView leftTrimmed = trimWhiteSpaceLeft(s);
    StrView bothTrimmed = trimWhiteSpaceRight(leftTrimmed);

    return bothTrimmed;
}

constexpr inline StrView cut(StrView s, char c, StrView& out) {
    out = core::sv();
    if (s.empty()) return core::sv();

    addr_off symbolIdx = -1;
    for (addr_size i = 0; i < s.len(); i++) {
        if (s[i] == c) {
            symbolIdx = addr_off(i);
            break;
        }
    }
    if (symbolIdx < 0) {
        return core::sv();
    }

    addr_off retLen = addr_off(s.len()) - symbolIdx - 1;
    StrView ret = retLen > 0
                  ? core::sv(s.data() + symbolIdx + 1, addr_size(retLen))
                  : core::sv();

    out = symbolIdx > 0
          ? core::sv(s.data(), addr_size(symbolIdx))
          : core::sv();

    return ret;
}

constexpr inline StrView skipWhiteSpace(StrView s) {
    if (s.empty()) return core::sv();

    addr_off i = 0;
    while (i < addr_off(s.len()) && isWhiteSpace(s[addr_size(i)])) {
        i++;
    }

    addr_off len = addr_off(s.len()) - i;
    StrView ret = len > 0
                  ? core::sv(s.data() + i, addr_size(len))
                  : core::sv();
    return ret;
}

constexpr inline StrView skip(StrView s, char c) {
    if (s.empty()) return core::sv();

    addr_off i = 0;
    while (i < addr_off(s.len()) && s[addr_size(i)] == c) {
        i++;
    }

    addr_off len = addr_off(s.len()) - i;
    StrView ret = len > 0
                  ? core::sv(s.data() + i, addr_size(len))
                  : core::sv();
    return ret;
}

constexpr inline bool startsWith(StrView s, const char* prefix) {
    return startsWith(s, core::sv(prefix));
}
constexpr inline bool startsWith(StrView s, StrView prefix) {
    if (s.empty() || prefix.empty()) return false;
    if (prefix.len() > s.len()) return false;

    for (addr_size i = 0; i < prefix.len(); i++) {
        if (s[i] != prefix[i]) return false;
    }

    return true;
}

constexpr inline bool endsWith(StrView s, const char* postfix) {
    return endsWith(s, core::sv(postfix));
}
constexpr inline bool endsWith(StrView s, StrView postfix) {
    if (s.empty() || postfix.empty()) return false;
    if (postfix.len() > s.len()) return false;

    for (addr_off i = addr_off(s.len()) - 1; i >= 0; i--) {
        if (s[addr_size(i)] != postfix[addr_size(i)]) return false;
    }

    return true;
}

} // namespace core

using core::operator""_sv; // use string literal globally.
