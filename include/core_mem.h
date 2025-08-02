#pragma once

#include "core_exec_ctx.h"
#include <core_API.h>
#include <core_assert.h>
#include <core_bits.h>
#include <core_cstr.h>
#include <core_traits.h>
#include <core_types.h>

#include <math/core_math.h>

#include <cstring>

// TODO2: [PERFORMANCE] most functions here could have better performance.

namespace core {

using namespace coretypes;

template <typename T> inline addr_size    imemcopy(T* dest, const T* src, addr_size len);
template <typename T> constexpr addr_size cmemcopy(T* dest, const T* src, addr_size len);
template <typename T> constexpr addr_size memcopy(T* dest, const T* src, addr_size len);

template <typename T> inline addr_size    imemset(T* dest, const T& v, addr_size len);
template <typename T> constexpr addr_size cmemset(T* dest, const T& v, addr_size len);
template <typename T> constexpr addr_size memset(T* dest, const T& v, addr_size len);

template <typename T> inline i32                    imemcmp(const T* a, addr_size lena, const T* b, addr_size lenb);
template <typename T> constexpr i32                 cmemcmpBytes(const T* a, addr_size lena, const T* b, addr_size lenb);
template <typename T> constexpr i32                 memcmp(const T* a, addr_size lena, const T* b, addr_size lenb);
template <typename T, typename CmpFn> constexpr i32 memcmp(const T* a, addr_size lena, const T* b, addr_size lenb, CmpFn cmpFn);
template <typename T> constexpr i32                 memcmp(const T* a, const T* b, addr_size len);
template <typename T, typename CmpFn> constexpr i32 memcmp(const T* a, const T* b, addr_size len, CmpFn cmpFn);

                      inline void    imemswap(void* a, void* b, addr_size len);
template <typename T> constexpr void memswap(T* a, T* b, addr_size len);
template <typename T> constexpr void swap(T& a, T& b);

                      constexpr addr_size   align(addr_size n);
template <typename T> constexpr T*          append(T* dst, const T& val);
                      inline addr_off       ptrDiff(const void* a, const void* b);
                      inline void*          ptrAdvance(void* ptr, addr_size off);
template <typename T> constexpr rawbytes<T> toBytes(const T& v);

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
    constexpr bool eq(const char* other, addr_size olen) const {
        bool areEqual = olen == length && this->cmp(other, olen) == 0;
        return areEqual;
    }
    constexpr bool eq(const char* other) const {
        addr_size olen = core::cstrLen(other);
        bool areEqual = olen == length && this->cmp(other, olen) == 0;
        return areEqual;
    }

    constexpr i32 cmp(const Memory& other) const {
        return core::memcmp<T>(ptr, length, other.ptr, other.length);
    }
    constexpr i32 cmp(const char* other, addr_size olen) const {
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

/**
 * \brief
 * Frees the given memory block using the specified allocator.
 *
 * This function deallocates the memory pointed to by `mem.ptr`, using the allocator associated with `allocatorId`.
 * It then nulls the pointer and resets the length to 0.
 *
 * \warning
 * - The allocator must be the same one used to allocate the memory, or the behavior is VERY undefined.
 */
template <typename T>
void memoryFree(core::Memory<T>&& mem, core::AllocatorId allocatorId) {
    if (mem.ptr == nullptr || mem.length == 0) return;
    auto actx = core::getAllocator(allocatorId);
    actx.free(mem.data(), mem.len(), sizeof(T));
    mem.ptr = nullptr;
    mem.length = 0;
}

/**
 * \brief
 * Moves a memory block to a new buffer of size `count` using the given allocator.
 *
 * Allocates a new zero-initialized buffer of type `T[count]`, copies the contents from the original block,
 * and frees the original memory using the same allocator. Leaves `mem` null and empty.
 *
 * \param mem         The source memory block (rvalue reference).
 * \param count       The number of elements to allocate in the new block.
 * \param allocatorId The allocator to use for the new allocation and for freeing the old one.
 *
 * \return A new Memory<T> object owning the newly allocated memory.
 *
 * \warning
 * - `count` must be greater than or equal to `mem.len()`, otherwise this is a logic error.
 * - The allocator must be the same one used to allocate the memory, or the behavior is VERY undefined.
 * - Use only with types that are standard layout, trivially copyable, and trivially move assignable.
 */
template <typename T>
[[nodiscard]] Memory<T> memoryMove(core::Memory<T>&& mem, addr_size count, core::AllocatorId allocatorId) {
    static_assert(std::is_standard_layout_v<T>, "T should be standard layout");
    static_assert(std::is_trivially_copy_assignable_v<T>, "T should be trivially copy assignable");
    static_assert(std::is_trivially_move_assignable_v<T>, "T should be trivially move assignable");

    Assert(count >= mem.len(), "Trying to shrink with memoryMove is not advisable!");

    if (count == 0) return mem;

    auto actx = core::getAllocator(allocatorId);

    core::Memory<T> ret;
    ret.ptr = reinterpret_cast<char*>(actx.zeroAlloc(count, sizeof(T)));
    ret.length = count;

    core::memcopy(ret.ptr, mem.data(),mem.len());

    memoryFree(std::move(mem), allocatorId);

    return ret;
}

#pragma region Mem Copy ------------------------------------------------------------------------------------------------

template <typename T> inline addr_size imemcopy(T* dest, const T* src, addr_size len) {
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
        pa[i] ^= pb[i];
        pb[i] ^= pa[i];
        pa[i] ^= pb[i];
    }
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
    imemswap(a, b, len);
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
    IS_CONST_EVALUATED {
        if constexpr (std::is_copy_assignable_v<T>) {
            T tmp = a;
            a = b;
            b = tmp;
            return;
        }
    }

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
