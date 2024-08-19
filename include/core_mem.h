#pragma once

#include <core_API.h>
#include <core_assert.h>
#include <core_traits.h>
#include <core_types.h>

namespace core {

using namespace coretypes;

template <typename T> inline T*          memcopy(T* dest, const T* src, addr_size len);
template <typename T> inline T*          memset(T* dest, u8 v, addr_size len);
                      constexpr i32      memcmp(const char* a, addr_size lena, const char* b, addr_size lenb);
                      constexpr i32      memcmp(const char* a, const char* b, addr_size len);
                      constexpr i32      memcmp(const uchar* a, addr_size lena, const uchar* b, addr_size lenb);
                      constexpr i32      memcmp(const uchar* a, const uchar* b, addr_size len);
template <typename T> i32                memcmp(const T* a, addr_size lena, const T* b, addr_size lenb);
template <typename T> i32                memcmp(const T* a, const T* b, addr_size len);
template <typename T> void               memswap(T* a, T* b, addr_size len);
template <typename T> void               memfill(T* dest, addr_size dstLen, const T& val);
template <typename T> addr_off           memidxof(const T* a, addr_size len, const T& val);
                      constexpr addr_off memidxof(const char* a, addr_size len, const char val);
                      constexpr addr_off memidxof(const char* a, addr_size lena, const char* vals, addr_size lenv);

                      constexpr addr_size align(addr_size n);
template <typename T> constexpr T*        append(T* dst, const T& val);
template <typename T> constexpr void      swap(T& a, T& b);
                      inline addr_size    ptrDiff(const void* a, const void* b);
                      inline void*        ptrAdvance(void* ptr, addr_size off);

namespace detail {

inline void* memcopyImpl(void* dest, const void* src, addr_size len) {
    // TODO2: [PERFORMANCE] William Chan has a good implementation of a fast memcpy.

    char* pdest = reinterpret_cast<char*>(dest);
    const char* psrc = reinterpret_cast<const char*>(src);

    for (addr_size i = 0; i < len; i++) {
        *pdest++ = *psrc++;
    }

    return pdest;
}

inline void* memsetImpl(void* dest, u8 u, addr_size len) {
    char* pdest = reinterpret_cast<char*>(dest);

    for (addr_size i = 0; i < len; i++) {
        *pdest++ = u;
    }

    return pdest;
}

constexpr i32 memcmpImpl(const char* a, addr_size lena, const char* b, addr_size lenb) {
    addr_size len = lena < lenb ? lena : lenb;
    for (addr_size i = 0; i < len; i++) {
        if (a[i] != b[i]) return a[i] - b[i];
    }

    if (lena > lenb) return 1;
    if (lena < lenb) return -1;
    else return 0;
}

constexpr i32 memcmpImpl(const uchar* a, addr_size lena, const uchar* b, addr_size lenb) {
    addr_size len = lena < lenb ? lena : lenb;
    for (addr_size i = 0; i < len; i++) {
        if (a[i] != b[i]) return a[i] - b[i];
    }

    if (lena > lenb) return 1;
    if (lena < lenb) return -1;
    else return 0;
}

template <typename T> void memswapImpl(T* a, T* b, addr_size len) {
    char* pa = reinterpret_cast<char*>(a);
    char* pb = reinterpret_cast<char*>(b);
    for (addr_size i = 0; i < len; i++) {
        swap(pa[i], pb[i]);
    }
}

template <typename T> addr_off memidxofImpl(const T* a, addr_size len, const T& val) {
    if (a == nullptr) return -1;
    const char* pa = reinterpret_cast<const char*>(a);
    const char* pv = reinterpret_cast<const char*>(&val);
    addr_off lena = addr_off(len * sizeof(T));
    addr_off lenv = addr_off(sizeof(val));
    for (addr_off i = 0; i <= lena - lenv; i += sizeof(T)) {
        i32 cmpVal = memcmp(pa + i, lenv, pv, lenv);
        if (cmpVal == 0) return i / sizeof(T);
    }
    return -1;
}

constexpr addr_off memidxofImpl(const char* a, addr_size lena, const char* vals, addr_size lenv) {
    if (a == nullptr || vals == nullptr) return -1;
    if (lena == 0 && lenv == 0) return 0;
    for (addr_off i = 0; i <= addr_off(lena) - addr_off(lenv); i++) {
        i32 cmpVal = memcmp(a + i, lenv, vals, lenv);
        if (cmpVal == 0) return i;
    }
    return -1;
}

} // namespace detail

template <typename T> inline T* memcopy(T* dest, const T* src, addr_size len) {
    if constexpr (!std::is_void_v<T>) { len *= sizeof(T); }
    return reinterpret_cast<T*>(detail::memcopyImpl(dest, src, len));
}
template <typename T> inline T* memset(T* dest, u8 v, addr_size len) {
    if constexpr (!std::is_void_v<T>) { len *= sizeof(T); }
    return reinterpret_cast<T*>(detail::memsetImpl(dest, v, len));
}

constexpr i32 memcmp(const char* a, addr_size lena, const char* b, addr_size lenb) {
    return detail::memcmpImpl(a, lena, b, lenb);
}
constexpr i32 memcmp(const char* a, const char* b, addr_size len) {
    return memcmp(a, len, b, len);
}
constexpr i32 memcmp(const uchar* a, addr_size lena, const uchar* b, addr_size lenb) {
    return detail::memcmpImpl(a, lena, b, lenb);
}
constexpr i32 memcmp(const uchar* a, const uchar* b, addr_size len) {
    return memcmp(a, len, b, len);
}
template <typename T> i32 memcmp(const T* a, addr_size lena, const T* b, addr_size lenb) {
    const char* pa = reinterpret_cast<const char*>(a);
    const char* pb = reinterpret_cast<const char*>(b);
    return memcmp(pa, lena, pb, lenb);
}
template <typename T> i32 memcmp(const T* a, const T* b, addr_size len) {
    return memcmp(a, len, b, len);
}

template <typename T> void memswap(T* a, T* b, addr_size len) {
    return detail::memswapImpl(a, b, len);
}

template <typename T>
void memfill(T* dest, addr_size dstLen, const T& val) {
    u8* p = reinterpret_cast<u8*>(dest);
    const u8* vbytes = reinterpret_cast<const u8*>(&val);
    addr_size vsize = sizeof(val);
    for (addr_size i = 0; i < dstLen * vsize; i+=vsize) {
        core::memcopy(p + i, vbytes, vsize);
    }
}

template <typename T> addr_off memidxof(const T* a, addr_size len, const T& val) {
    return detail::memidxofImpl(a, len, val);
}

constexpr addr_off memidxof(const char* a, addr_size len, char val) {
    return memidxof(a, len, &val, 1);
}

constexpr addr_off memidxof(const char* a, addr_size lena, const char* vals, addr_size lenv) {
    return detail::memidxofImpl(a, lena, vals, lenv);
}

constexpr addr_size align(addr_size n) {
    return (n + sizeof(addr_size) - 1) & ~(sizeof(addr_size) - 1);
}

template <typename T>
constexpr T* append(T* dst, const T& val) {
    *dst = val;
    return dst + 1;
}

template <typename T>
constexpr void swap(T& a, T& b) {
    T tmp = a;
    a = b;
    b = tmp;
}

inline addr_size ptrDiff(const void* a, const void* b) {
    return reinterpret_cast<addr_size>(a) - reinterpret_cast<addr_size>(b);
}

inline void* ptrAdvance(void* ptr, addr_size off) {
    return reinterpret_cast<void*>(reinterpret_cast<char*>(ptr) + off);
}

} // namespace core
