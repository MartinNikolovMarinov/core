#pragma once

#include <core_traits.h>
#include <core_types.h>

#include <cstring>

namespace core {

using namespace coretypes;

                        constexpr inline bool isDigit(char c);
                        constexpr inline bool isHexDigit(char c);
                        constexpr inline bool isWhiteSpace(char c);

template <typename T>   constexpr inline T    charToDigit(char s);
template<typename TInt> constexpr inline char digitToChar(TInt digit);

                        constexpr inline char toLowerCaseASCII(char c);
                        constexpr inline char toUpperCaseASCII(char c);
                        constexpr inline bool isLowerCaseASCII(char c);
                        constexpr inline bool isUpperCaseASCII(char c);

                        constexpr inline addr_size cstrLen(const char* p);
                        constexpr inline addr_size cstrLen(const uchar* p);

                        constexpr const char* cstrSkipSpace(const char* s);
                        constexpr       char* cstrSkipSpace(char* s);

                                  inline i32  cstrCmpUnsafe(const char* a, const char* b);
                                  inline bool cstrEqUnsafe(const char* a, const char* b);
                        constexpr inline i32  cstrCmpIgnoreCaseANSI(char a, char b);


constexpr inline bool isDigit(char c) { return c >= '0' && c <= '9'; }
constexpr inline bool isHexDigit(char c) { return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'); }
constexpr inline bool isWhiteSpace(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }

template <typename T>
constexpr inline T charToDigit(char s) {
    static_assert(std::is_arithmetic_v<T>, "T must be an arithmetic type.");
    return T(s - '0');
}

template<typename T>
constexpr inline char digitToChar(T digit) {
    return char(digit % 10) + '0';
}

constexpr inline char toLowerCaseASCII(char c) { return (c | 0x20); }
constexpr inline char toUpperCaseASCII(char c) { return (c & ~0x20); }

constexpr inline bool isLowerCaseASCII(char c) { return c == toLowerCaseASCII(c); }
constexpr inline bool isUpperCaseASCII(char c) { return c == toUpperCaseASCII(c); }

constexpr inline i32 cstrCmpIgnoreCaseANSI(char a, char b) { return i32(toLowerCaseASCII(a) - toLowerCaseASCII(b)); }

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

inline i32 cstrCmpUnsafe(const char* a, const char* b) {
    addr_size lena = cstrLen(a);
    addr_size lenb = cstrLen(b);
    addr_size len = lena < lenb ? lena : lenb;
    i32 res = std::strncmp(a, b, len);
    if (res == 0) {
        if (lena > lenb) return 1;
        if (lena < lenb) return -1;
    }
    return res;
}

inline bool cstrEqUnsafe(const char* a, const char* b) {
    return cstrCmpUnsafe(a, b) == 0;
}

} // namespace core
