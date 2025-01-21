#pragma once

#include <core_API.h>
#include <core_assert.h>
#include <core_traits.h>
#include <core_types.h>

#include <cstring>

namespace core {

using namespace coretypes;

template <typename T> constexpr T*       memcopy(T* dest, const T* src, addr_size len);
                      inline void*       memset(void* dest, u8 v, addr_size len);
template <typename T> constexpr T*       memset(T* dest, const T& v, addr_size len);
                      constexpr i32      memcmp(const char* a, addr_size lena, const char* b, addr_size lenb);
                      constexpr i32      memcmp(const char* a, const char* b, addr_size len);
                      constexpr i32      memcmp(const uchar* a, addr_size lena, const uchar* b, addr_size lenb);
                      constexpr i32      memcmp(const uchar* a, const uchar* b, addr_size len);
template <typename T> i32                memcmp(const T* a, addr_size lena, const T* b, addr_size lenb);
template <typename T> i32                memcmp(const T* a, const T* b, addr_size len);
template <typename T> constexpr void     memswap(T* a, T* b, addr_size len);
template <typename T> addr_off           memidxof(const T* a, addr_size len, const T& val);
                      constexpr addr_off memidxof(const char* a, addr_size len, const char val);
                      constexpr addr_off memidxof(const char* a, addr_size lena, const char* vals, addr_size lenv);

                      constexpr addr_size align(addr_size n);
template <typename T> constexpr T*        append(T* dst, const T& val);
template <typename T> constexpr void      swap(T& a, T& b);
                      inline addr_off     ptrDiff(const void* a, const void* b);
                      inline void*        ptrAdvance(void* ptr, addr_size off);

template <typename T>
struct Memory {
    using size_type = addr_size;

    constexpr Memory() : ptr(nullptr), length(0) {}
    constexpr Memory(T* _ptr, size_type _len) : ptr(_ptr), length(_len) {}
    constexpr Memory(const Memory&) = default;
    constexpr Memory(Memory&& other) {
        if (this == &other) return;
        ptr = other.ptr;
        length = other.length;
        other.ptr = nullptr;
        other.length = 0;
    }

    constexpr Memory& operator=(const Memory&) = default;
    constexpr Memory& operator=(Memory&& other) {
        if (this == &other) return *this;
        ptr = other.ptr;
        length = other.length;
        other.ptr = nullptr;
        other.length = 0;
        return *this;
    }

    constexpr T* data() const { return ptr; }
    constexpr size_type len() const { return length; }

    constexpr operator bool() const { return ptr != nullptr; }

    constexpr T& operator[](size_type idx) const { return ptr[idx]; }
    constexpr T* atUnsafe(size_type idx) const { return ptr + idx; }
    constexpr T* at(size_type idx) const {
        if (idx < length) return atUnsafe(idx);
        else return nullptr;
    }

    constexpr bool eq(const Memory& other) const {
        bool areEqual = other.length == length && this->cmp(other) == 0;
        return areEqual;
    }

    constexpr i32 cmp(const Memory& other) const {
        return core::memcmp<T>(ptr, length, other.ptr, other.length);
    }

    constexpr bool contains(const T& v) const {
        for (size_type i = 0; i < len(); i++) {
            if (ptr[i] == v) {
                return true;
            }
        }
        return false;
    }

    constexpr Memory slice(size_type offset) {
        Assert(offset < length, "offset for slice outside memory boundary");
        return Memory(ptr + offset, length - offset);
    }

    constexpr Memory slice(size_type offset, size_type slen) {
        if (slen == 0) return Memory(nullptr, 0);
        Assert(offset < length, "offset for slice outside memory boundary");
        Assert(slen <= length, "slen for slice outside memory boundary");
        Assert(offset + slen <= length, "slice outside memory boundary");
        return Memory(ptr + offset, slen);
    }

    T* ptr;
    size_type length;
};

namespace detail {

inline void* memcopyImpl(void* dest, const void* src, addr_size len) {
    // TODO2: [PERFORMANCE] vectorize to acheave better performance.
    u8* ret = reinterpret_cast<u8*>(std::memcpy(dest, src, len));
    return ret + len;
}

inline void* memsetImpl(void* dest, u8 u, addr_size len) {
    // TODO2: [PERFORMANCE] vectorize to acheave better performance.
    u8* ret = reinterpret_cast<u8*>(std::memset(dest, u, len));
    return ret + len;
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

inline void memswapImpl(void* a, void* b, addr_size len) {
    char* pa = reinterpret_cast<char*>(a);
    char* pb = reinterpret_cast<char*>(b);

    for (addr_size i = 0; i < len; ++i) {
        pa[i] ^= pb[i];
        pb[i] ^= pa[i];
        pa[i] ^= pb[i];
    }
}

template <typename T> addr_off memidxofImpl(const T* a, addr_size len, const T& val) {
    if (a == nullptr) return -1;
    const char* pa = reinterpret_cast<const char*>(a);
    const char* pv = reinterpret_cast<const char*>(&val);
    addr_off lena = addr_off(len * sizeof(T));
    addr_off lenv = addr_off(sizeof(val));
    for (addr_off i = 0; i <= lena - lenv; i += sizeof(T)) {
        i32 cmpVal = memcmp(pa + i, addr_size(lenv), pv, addr_size(lenv));
        if (cmpVal == 0) return i / addr_off(sizeof(T));
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

template <typename T> constexpr T* memcopy(T* dest, const T* src, addr_size len) {
    IS_CONST_EVALUATED {
        if constexpr (std::is_copy_assignable_v<T>) {
            for (addr_size i = 0; i < len; i++) {
                *dest++ = *src++; // NOTE: T needs to have an equals operator.
            }
            return dest;
        }
    }

    // The below code can byte copy any T.
    if constexpr (!std::is_void_v<T>) len *= sizeof(T);
    return reinterpret_cast<T*>(detail::memcopyImpl(dest, src, len));
}

inline void* memset(void* dest, u8 v, addr_size len) {
    return detail::memsetImpl(dest, u8(v), len);
}

template <typename T> constexpr T* memset(T* dest, const T& v, addr_size len) {
    if (len == 0) return dest;

    IS_NOT_CONST_EVALUATED {
        // In non constexpr context and T-s of size 1 byte prefer using fast memset variant.
        if constexpr (sizeof(T) == sizeof(u8)) {
            return reinterpret_cast<T*>(detail::memsetImpl(dest, u8(v), len));
        }
    }

    // Fallback for constexpr contexts and types larger than 1 byte.
    static_assert(std::is_copy_assignable_v<T>);
    for (addr_size i = 0; i < len; i++) {
        *dest++ = v;
    }
    return dest;
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
    // TODO2: This can be modified to have a constexpr version, if needed.
    const char* pa = reinterpret_cast<const char*>(a);
    const char* pb = reinterpret_cast<const char*>(b);
    return memcmp(pa, lena * sizeof(T), pb, lenb * sizeof(T));
}
template <typename T> i32 memcmp(const T* a, const T* b, addr_size len) {
    return memcmp(a, len, b, len);
}

template <typename T>
constexpr void memswap(T* a, T* b, addr_size len) {
    if (a == b) return;

    IS_CONST_EVALUATED {
        // Slow element-wise swapping for constexpr contexts
        for (addr_size i = 0; i < len; ++i) {
            swap(a[i], b[i]);
        }
        return;
    }

    // The below code can byte copy any T.
    if constexpr (!std::is_void_v<T>) len *= sizeof(T);
    detail::memswapImpl(a, b, len);
}

template <typename T> addr_off memidxof(const T* a, addr_size len, const T& val) {
    // TODO2: This can be modified to have a constexpr version, if needed.
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
    IS_CONST_EVALUATED {
        if constexpr (std::is_copy_assignable_v<T>) {
            T tmp = a;
            a = b;
            b = tmp;
            return;
        }
    }

    // TODO2: [PERFORMANCE] vectorize to acheave better performance.

    // Byte-wise swap using XOR for runtime context
    char* pa = reinterpret_cast<char*>(&a);
    char* pb = reinterpret_cast<char*>(&b);
    for (std::size_t i = 0; i < sizeof(T); ++i) {
        pa[i] ^= pb[i];
        pb[i] ^= pa[i];
        pa[i] ^= pb[i];
    }
}

inline addr_off ptrDiff(const void* a, const void* b) {
    // bit casting pointers is not allowed, so reinterpret_cast is needed here.
    return reinterpret_cast<addr_off>(a) - reinterpret_cast<addr_off>(b);
}

inline void* ptrAdvance(void* ptr, addr_size off) {
    // bit casting pointers is not allowed, so reinterpret_cast is needed here.
    return reinterpret_cast<void*>(reinterpret_cast<char*>(ptr) + off);
}

} // namespace core
