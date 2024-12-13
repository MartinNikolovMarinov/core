#pragma once

#include <core_mem.h>
#include <core_traits.h>
#include <core_types.h>

// TODO2: [PERFORMANCE] Everything in this file can be much faster.

namespace core {

using namespace coretypes;

                        constexpr inline bool isDigit(char c);
                        constexpr inline bool isHexDigit(char c);
                        constexpr inline bool isWhiteSpace(char c);

template <typename T>   constexpr inline T    toDigit(char s);
template<typename TInt> constexpr inline char digitToChar(TInt digit);

                        constexpr inline char toLowerCaseANSI(char c);
                        constexpr inline char toUpperCaseANSI(char c);
                        constexpr inline bool isLowerCaseANSI(char c);
                        constexpr inline bool isUpperCaseANSI(char c);

                        constexpr inline addr_size cstrLen(const char* p);
                        constexpr inline addr_size cstrLen(const uchar* p);

                        constexpr const char* cstrSkipSpace(const char* s);
                        constexpr       char* cstrSkipSpace(char* s);

constexpr inline bool isDigit(char c) { return c >= '0' && c <= '9'; }
constexpr inline bool isHexDigit(char c) { return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'); }
constexpr inline bool isWhiteSpace(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }

template <typename T>
constexpr inline T toDigit(char s) {
    static_assert(std::is_arithmetic_v<T>, "T must be an arithmetic type.");
    return T(s - '0');
}

template<typename T>
constexpr inline char digitToChar(T digit) {
    return char(digit % 10) + '0';
}

constexpr inline char toLowerCaseANSI(char c) { return (c | 0x20); }
constexpr inline char toUpperCaseANSI(char c) { return (c & ~0x20); }

constexpr inline bool isLowerCaseANSI(char c) { return c == toLowerCaseANSI(c); }
constexpr inline bool isUpperCaseANSI(char c) { return c == toUpperCaseANSI(c); }

constexpr inline i32 cmpIgnoreCaseANSI(char a, char b) { return i32(toLowerCaseANSI(a) - toLowerCaseANSI(b)); }

namespace detail {

template<typename TChar>
constexpr inline addr_size cstrLenImpl(const TChar* p) {
    static_assert(is_char_v<TChar>, "TChar must be a char type.");
    if (p == nullptr) return 0;
    const TChar* start = p;
    while(*p) p++;
    return addr_size(p - start);
}

} // namespace detail

constexpr inline addr_size cstrLen(const char* p)  { return detail::cstrLenImpl(p); }
constexpr inline addr_size cstrLen(const uchar* p) { return detail::cstrLenImpl(p); }

constexpr inline const char* cstrSkipSpace(const char* s) {
    if (s == nullptr) return s;
    while (isWhiteSpace(*s)) s++;
    return s;
}

constexpr inline char* cstrSkipSpace(char* s) {
    if (s == nullptr) return s;
    while (isWhiteSpace(*s)) s++;
    return s;
}

} // namespace core
