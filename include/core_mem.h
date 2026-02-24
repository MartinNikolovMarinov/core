#pragma once

#include <core_API.h>
#include <core_assert.h>
#include <core_bits.h>
#include <core_cstr.h>
#include <core_exec_ctx.h>
#include <core_traits.h>
#include <core_types.h>

#include <math/core_math.h>

#include <cstring>
#include <utility>

// TODO2: [PERFORMANCE] most functions here could have better performance.

namespace core {

using namespace coretypes;

template <typename T> struct Memory;
template <typename T> struct BufferedMemory;

template <typename T> inline    addr_size imemcopy(T* dest, const T* src, addr_size len);
template <typename T> constexpr addr_size cmemcopy(T* dest, const T* src, addr_size len);
template <typename T> constexpr addr_size memcopy(T* dest, const T* src, addr_size len);

template <typename T> inline    addr_size imemset(T* dest, const T& v, addr_size len);
template <typename T> constexpr addr_size cmemset(T* dest, const T& v, addr_size len);
template <typename T> constexpr addr_size memset(T* dest, const T& v, addr_size len);

template <typename T>                 inline i32    imemcmp(const T* a, addr_size lena, const T* b, addr_size lenb);
template <typename T>                 constexpr i32 cmemcmpBytes(const T* a, addr_size lena, const T* b, addr_size lenb);
template <typename T>                 constexpr i32 memcmp(const T* a, addr_size lena, const T* b, addr_size lenb);
template <typename T, typename CmpFn> constexpr i32 memcmp(const T* a, addr_size lena, const T* b, addr_size lenb, CmpFn cmpFn);
template <typename T>                 constexpr i32 memcmp(const T* a, const T* b, addr_size len);
template <typename T, typename CmpFn> constexpr i32 memcmp(const T* a, const T* b, addr_size len, CmpFn cmpFn);

                      inline void    imemswap(void* a, void* b, addr_size len);
template <typename T> constexpr void memswap(T* a, T* b, addr_size len);
template <typename T> constexpr void swap(T& a, T& b);

                      constexpr addr_size   align(addr_size n);
                      constexpr addr_size   align(addr_size n, u32 alignment);
template <typename T> constexpr T*          append(T* dst, const T& val);
                      inline addr_off       ptrDiff(const void* a, const void* b);
                      inline void*          ptrAdvance(void* ptr, addr_size off);
template <typename T> constexpr rawbytes<T> toBytes(const T& v);

template <typename T>
struct Memory {
    using size_type = addr_size;

    T* ptr;
    size_type length;

    constexpr T* data() const { return ptr; }
    constexpr size_type len() const { return length; }

    constexpr operator bool() const { return ptr != nullptr; }

    constexpr T& operator[](i32 idx) const { return ptr[idx]; }
    constexpr T& operator[](size_type idx) const { return ptr[idx]; }
    constexpr T& atUnsafe(i32 idx) const { return ptr[idx]; }
    constexpr T& atUnsafe(size_type idx) const { return ptr[idx]; }
    constexpr T& at(i32 idx) const { return at(size_type(idx)); }
    constexpr T& at(size_type idx) const {
        Assert(idx < length, "Index out of range");
        return atUnsafe(idx);
    }

    constexpr T* end() {
        Assert(ptr != nullptr, "memory is not initialized");
        return ptr + length;
    }
    constexpr const T* end() const {
        Assert(ptr != nullptr, "memory is not initialized");
        return ptr + length;
    }

    constexpr T* begin() {
        Assert(ptr != nullptr, "memory is not initialized");
        return ptr;
    }
    constexpr const T* begin() const {
        Assert(ptr != nullptr, "memory is not initialized");
        return ptr;
    }

    constexpr T& last() { return *(end() - 1); }
    constexpr const T& last() const { return *(end() - 1); }

    constexpr T& first() { return *(begin()); }
    constexpr const T& first() const { return *(begin()); }

    constexpr bool empty() const { return ptr == nullptr || length == 0; }

    constexpr void clear() {
        ptr = nullptr;
        length = 0;
    }

    constexpr bool eq(const Memory& other) const {
        bool areEqual = other.length == length && this->cmp(other) == 0;
        return areEqual;
    }
    constexpr bool eq(const char* other, size_type olen) const {
        bool areEqual = olen == length && this->cmp(other, olen) == 0;
        return areEqual;
    }
    constexpr bool eq(const char* other) const {
        size_type olen = core::cstrLen(other);
        bool areEqual = olen == length && this->cmp(other, olen) == 0;
        return areEqual;
    }

    constexpr i32 cmp(const Memory& other) const {
        return core::memcmp<T>(ptr, length, other.ptr, other.length);
    }
    constexpr i32 cmp(const char* other, size_type olen) const {
        return core::memcmp<T>(ptr, length, other, olen);
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
        return { ptr + offset, length - offset };
    }

    constexpr Memory slice(size_type offset, size_type slen) {
        if (slen == 0) return Memory(nullptr, 0);
        Assert(offset < length, "offset for slice outside memory boundary");
        Assert(slen <= length, "slen for slice outside memory boundary");
        Assert(offset + slen <= length, "slice outside memory boundary");
        return { ptr + offset, slen };
    }

    constexpr void setRange(size_type from, size_type _len, T&& v) {
        Assert(from + _len <= length, "selected range is outside memory boundary");
        core::memset(ptr + from, v, addr_size(_len));
    }
};

static_assert(std::is_trivial_v<Memory<i32>>, "Memory must be a trivial type.");

template<typename T>
struct BufferedMemory {
    using size_type = addr_size;

    static_assert(std::is_trivially_copyable_v<T>, "T should be trivially copyable");

    Memory<T> mem;
    addr_size at;

    constexpr size_type cap() { return mem.len(); }

    constexpr operator bool() const { return mem.ptr != nullptr; }

    constexpr T& operator[](i32 idx) const { return mem[idx]; }
    constexpr T& operator[](size_type idx) const { return mem[idx]; }

    constexpr T* end() {
        Assert(mem.ptr != nullptr, "memory is not initialized");
        return mem.ptr + at;
    }
    constexpr const T* end() const {
        Assert(mem.ptr != nullptr, "memory is not initialized");
        return mem.ptr + at;
    }

    constexpr T* begin() {
        Assert(mem.ptr != nullptr, "memory is not initialized");
        return mem.ptr;
    }
    constexpr const T* begin() const {
        Assert(mem.ptr != nullptr, "memory is not initialized");
        return mem.ptr;
    }

    constexpr T& last() { return *(end() - 1); }
    constexpr const T& last() const { return *(end() - 1); }

    constexpr T& first() { return *(begin()); }
    constexpr const T& first() const { return *(begin()); }

    constexpr bool empty() const { return mem.ptr == nullptr || at == 0; }

    inline void append(T&& x, core::AllocatorId allocatorId) {
        auto& actx = core::getAllocator(allocatorId);
        append(std::move(x), actx);
    }
    constexpr void append(T&& x, core::AllocatorContext& actx) {
        if (at >= cap()) {
            size_type newCap = cap() == 0 ? 1 : cap();
            if (at >= newCap) {
                newCap *= 2;
            }

            reallocWith(newCap, actx);
        }

        mem.ptr[at] = std::move(x);
        at++;
    }

    constexpr void removeUnordered(size_type i) {
        Assert(i < at, "index out of range");
        Assert(!mem.empty(), "[BUG] this should never happen");
        core::swap(mem[i], mem[at - 1]);
        at--;
    }

    inline void allocWith(size_type count, core::AllocatorId allocatorId) {
        auto& actx = core::getAllocator(allocatorId);
        allocWith(count, actx);
    }
    constexpr void allocWith(size_type count, core::AllocatorContext& actx) {
        mem.ptr = reinterpret_cast<T*>(actx.alloc(count, sizeof(T)));
        mem.length = count;
        at = 0;
    }

    inline void callocWith(size_type count, core::AllocatorId allocatorId) {
        auto& actx = core::getAllocator(allocatorId);
        callocWith(count, actx);
    }
    constexpr void callocWith(size_type count, core::AllocatorContext& actx) {
        mem.ptr = reinterpret_cast<T*>(actx.zeroAlloc(count, sizeof(T)));
        mem.length = count;
        at = 0;
    }

    inline void reallocWith(size_type count, core::AllocatorId allocatorId) {
        auto& actx = core::getAllocator(allocatorId);
        reallocWith(count, actx);
    }
    constexpr void reallocWith(size_type count, core::AllocatorContext& actx) {
        if (count == 0) return;

        mem.ptr = reinterpret_cast<T*>(actx.reallocate(mem.ptr, count, sizeof(T), mem.length, sizeof(T)));
        mem.length = count;
    }

    inline void freeWith(core::AllocatorId allocatorId) {
        auto& actx = core::getAllocator(allocatorId);
        freeWith(actx);
    }
    constexpr void freeWith(core::AllocatorContext& actx) {
        if (mem.empty()) return;
        actx.free(mem.ptr, mem.length, sizeof(T));
        mem.clear();
        at = 0;
    }
};

static_assert(std::is_trivial_v<BufferedMemory<i32>>, "BufferedMemory must be a trivial type.");

#pragma region Mem Copy ------------------------------------------------------------------------------------------------

template <typename T> inline addr_size imemcopy(T* dest, const T* src, addr_size len) {
    static_assert(std::is_trivially_copyable_v<T>, "memcopy requires trivially copyable types");

    if constexpr (sizeof(T) == sizeof(u8)) {
        std::memcpy(dest, src, len);
        return len;
    }
    else {
        // Byte-wise copy:
        char* pdest = reinterpret_cast<char*>(dest);
        const char* psrc = reinterpret_cast<const char*>(src);
        for (addr_size i = 0; i < len * sizeof(T); i++) {
            pdest[i] = psrc[i];
        }

        return len;
    }
}

template <typename T> constexpr addr_size cmemcopy(T* dest, const T* src, addr_size len) {
    static_assert(std::is_trivial_v<T>);

    for (addr_size i = 0; i < len; i++) {
        dest[i] = src[i];
    }

    return len;
}

template <typename T> constexpr addr_size memcopy(T* dest, const T* src, addr_size len) {
    static_assert(std::is_trivially_copyable_v<T>, "memcopy requires trivially copyable types");

    IS_CONST_EVALUATED {
        if constexpr (std::is_trivial_v<T>) {
            return cmemcopy(dest, src, len);
        }
    }

    return imemcopy(dest, src, len);
}

#pragma endregion Mem Copy ---------------------------------------------------------------------------------------------

#pragma region Mem Set -------------------------------------------------------------------------------------------------

template <typename T> inline addr_size imemset(T* dest, const T& v, addr_size len) {
    static_assert(sizeof(T) == sizeof(u8));
    std::memset(dest, v, len);
    return len;
}

template <typename T> constexpr addr_size cmemset(T* dest, const T& v, addr_size len) {
    static_assert(std::is_copy_assignable_v<T>);
    for (addr_size i = 0; i < len; i++) {
        dest[i] = v;
    }
    return len;
}

template <typename T> constexpr addr_size memset(T* dest, const T& v, addr_size len) {
    IS_NOT_CONST_EVALUATED {
        if constexpr (sizeof(T) == sizeof(u8)) {
            return imemset(dest, v, len);
        }
    }

    return cmemset(dest, v, len);
}

#pragma endregion Mem Set ----------------------------------------------------------------------------------------------

#pragma region Mem Compare ---------------------------------------------------------------------------------------------

template <typename T> inline i32 imemcmp(const T* a, addr_size lena, const T* b, addr_size lenb) {
    addr_size len = lena < lenb ? lena : lenb;
    i32 res = std::memcmp(a, b, len * sizeof(T));
    if (res == 0) {
        if (lena > lenb) return 1;
        if (lena < lenb) return -1;
    }
    return res;
}

template <typename T> constexpr i32 cmemcmpBytes(const T* a, addr_size lena, const T* b, addr_size lenb) {
    static_assert(sizeof(T) == sizeof(u8));
    addr_size len = (lena < lenb ? lena : lenb);
    for (addr_size i = 0; i < len; ++i) {
        if (a[i] != b[i]) {
            return i32(a[i]) - i32(b[i]);
        }
    }
    if (lena > lenb) return 1;
    if (lena < lenb) return -1;
    return 0;
}

template <typename T, typename CmpFn> constexpr i32 cmemcmp(const T* a, addr_size lena, const T* b, addr_size lenb, CmpFn cmpFn) {
    addr_size len = (lena < lenb ? lena : lenb);
    for (addr_size i = 0; i < len; ++i) {
        i32 ret = cmpFn(a[i], b[i]);
        if (ret != 0) {
            return ret;
        }
    }
    if (lena > lenb) return 1;
    if (lena < lenb) return -1;
    return 0;
}

template <typename T> constexpr i32 memcmp(const T* a, addr_size lena, const T* b, addr_size lenb) {
    IS_CONST_EVALUATED {
        if constexpr (sizeof(T) == sizeof(u8)) {
            return cmemcmpBytes(a, lena, b, lenb);
        }
        else if constexpr (std::is_integral_v<T>) {
            return cmemcmp(a, lena, b, lenb, [](const T& x, const T& y) -> i32 {
                if (x > y)      return 1;
                else if (x < y) return -1;
                else            return 0;
            });
        }
    }

    return imemcmp(a, lena, b, lenb);
}

template <typename T, typename CmpFn> constexpr i32 memcmp(const T* a, addr_size lena, const T* b, addr_size lenb, CmpFn cmpFn) {
    IS_CONST_EVALUATED {
        return cmemcmp<T, CmpFn>(a, lena, b, lenb, cmpFn);
    }

    return imemcmp(a, lena, b, lenb);
}

template <typename T> constexpr i32 memcmp(const T* a, const T* b, addr_size len) {
    return memcmp(a, len, b, len);
}

template <typename T, typename CmpFn> constexpr i32 memcmp(const T* a, const T* b, addr_size len, CmpFn cmpFn) {
    return memcmp(a, len, b, len, cmpFn);
}

#pragma endregion Mem Compare ------------------------------------------------------------------------------------------

inline void imemswap(void* a, void* b, addr_size len) {
    char* pa = reinterpret_cast<char*>(a);
    char* pb = reinterpret_cast<char*>(b);

    for (addr_size i = 0; i < len; ++i) {
        char tmp = pa[i];
        pa[i] = pb[i];
        pb[i] = tmp;
    }
}

template <typename T>
constexpr void memswap(T* a, T* b, addr_size len) {
    static_assert(std::is_trivially_copyable_v<T>, "memswap requires trivially copyable types");

    IS_CONST_EVALUATED {
        for (addr_size i = 0; i < len; ++i) {
            auto tmp = a[i];
            a[i] = b[i];
            b[i] = tmp;
        }
        return;
    }

    addr_size byteLen = len;
    if constexpr (!std::is_void_v<T>) byteLen *= sizeof(T);
    imemswap(a, b, byteLen);
}

constexpr addr_size align(addr_size n) {
    return (n + sizeof(addr_size) - 1) & ~(sizeof(addr_size) - 1);
}

constexpr addr_size align(addr_size n, u32 alignment) {
    Assert(alignment > 0, "Alignment must be non-zero");
    Assert(core::ispow2(alignment), "Alignment must be a power of 2");
    return (n + alignment - 1) & ~(alignment - 1);
}

template <typename T>
constexpr T* append(T* dst, const T& val) {
    *dst = val;
    return dst + 1;
}

template <typename T>
constexpr void swap(T& a, T& b) {
    static_assert(std::is_trivially_copyable_v<T>, "swap requires trivially copyable types");
    IS_CONST_EVALUATED {
        T tmp = a;
        a = b;
        b = tmp;
        return;
    }

    memswap(&a, &b, 1);
}

inline addr_off ptrDiff(const void* a, const void* b) {
    const char* pa = reinterpret_cast<const char*>(a);
    const char* pb = reinterpret_cast<const char*>(b);
    return addr_off(pa - pb);
}

inline void* ptrAdvance(void* ptr, addr_size off) {
    // bit casting pointers is not allowed, so reinterpret_cast is needed here.
    return reinterpret_cast<void*>(reinterpret_cast<char*>(ptr) + off);
}

/**
 * IMPORTANT:
 *  This function does not work very well when the type T has padding between members. In that case iterating over the
 *  bytes might result in accessing uninitialized (padding) bytes and that can cause tools like valgrind to complain.
 *  You have to keep in mind the exact layout of T when iterating.
 *
 *  This function should be used sparingly.
*/
template <typename T> constexpr rawbytes<T> toBytes(const T& v) {
    auto ret = core::bitCast<rawbytes<T>>(v);
    return ret;
}

} // namespace core
