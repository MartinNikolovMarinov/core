#pragma once

#include <core_mem.h>
#include <core_traits.h>
#include <core_types.h>

// TODO2: [PERFORMANCE] Everything in this file can be much faster.

namespace core {

using namespace coretypes;

constexpr inline bool isDigit(char c) { return c >= '0' && c <= '9'; }
constexpr inline bool isWhiteSpace(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }

namespace detail {

template<typename TChar>
constexpr addr_size cstrLenImpl(const TChar* p) {
    static_assert(is_char_v<TChar>, "TChar must be a char type.");
    if (p == nullptr) return 0;
    const TChar* start = p;
    while(*p) p++;
    return addr_size(p - start);
}

} // namespace detail

constexpr addr_size cstrLen(const char* p)  { return detail::cstrLenImpl(p); }
constexpr addr_size cstrLen(const uchar* p) { return detail::cstrLenImpl(p); }

constexpr const char* cstrSkipSpace(const char* s) {
    if (s == nullptr) return s;
    while (isWhiteSpace(*s)) s++;
    return s;
}

constexpr char* cstrSkipSpace(char* s) {
    if (s == nullptr) return s;
    while (isWhiteSpace(*s)) s++;
    return s;
}

} // namespace core
