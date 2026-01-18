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

template <typename T> void memoryFree(Memory<T>&& mem, AllocatorId allocatorId);
template <typename T> void memoryFree(Memory<T>&& mem, AllocatorContext& actx);
template <typename T> [[nodiscard]] Memory<T> memoryReallocate(Memory<T>&& mem, addr_size count, AllocatorId allocatorId);
template <typename T> [[nodiscard]] Memory<T> memoryReallocate(Memory<T>&& mem, addr_size count, AllocatorContext& actx);
template <typename T> [[nodiscard]] Memory<T> memorySet(Memory<T>& mem, addr_size i, T&& v, AllocatorId allocatorId);
template <typename T> [[nodiscard]] Memory<T> memorySet(Memory<T>& mem, addr_size i, T&& v, AllocatorContext& actx);
template <typename T> [[nodiscard]] Memory<T> memoryZeroAllocate(addr_size count, AllocatorId allocatorId);
template <typename T> [[nodiscard]] Memory<T> memoryZeroAllocate(addr_size count, AllocatorContext& actx);

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
    constexpr T* atUnsafe(size_type idx) const { return ptr + idx; }
    constexpr T& at(i32 idx) const { return at(addr_size(idx)); }
    constexpr T* at(size_type idx) const {
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

    constexpr T* last() { return end() - 1; }
    constexpr const T* last() const { return end() - 1; }

    constexpr bool empty() const { return ptr == nullptr || length == 0; }

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
};

static_assert(std::is_trivial_v<Memory<i32>>, "Memory must be a trivial type.");

/**
 * \brief
 * Allocates a memory block using the specified allocator. The allocated memory is zeroed-out.
 */
template <typename T>
[[nodiscard]] core::Memory<T> memoryZeroAllocate(addr_size count, core::AllocatorId allocatorId) {
    auto& actx = core::getAllocator(allocatorId);
    return memoryZeroAllocate<T>(count, actx);
}
template <typename T>
[[nodiscard]] core::Memory<T> memoryZeroAllocate(addr_size count, core::AllocatorContext& actx) {
    auto fileMemory = core::Memory<T> (
        reinterpret_cast<T*>(actx.zeroAlloc(count, sizeof(T))),
        count
    );
    return fileMemory;
}

/**
 * \brief
 * Frees the given memory block using the specified allocator.
 *
 * This function deallocates the memory pointed to by `mem.ptr`, using the allocator associated with `allocatorId`.
 * It then nulls the pointer and resets the length to 0.
 *
 * \warning
 * - The allocator must be the same one used to allocate the memory, or the behavior is VERY undefined.
 * - Use only with types that are trivially destructible (no destructors are invoked).
 */
template <typename T>
void memoryFree(core::Memory<T>&& mem, core::AllocatorId allocatorId) {
    static_assert(std::is_trivially_destructible_v<T>, "T should be trivially destructible");

    auto& actx = core::getAllocator(allocatorId);
    memoryFree(std::move(mem), actx);
}
template <typename T>
void memoryFree(core::Memory<T>&& mem, core::AllocatorContext& actx) {
    static_assert(std::is_trivially_destructible_v<T>, "T should be trivially destructible");

    if (mem.empty()) return;
    actx.free(mem.data(), mem.len(), sizeof(T));
    mem.ptr = nullptr;
    mem.length = 0;
}

/**
 * \brief
 * Reallocates a memory block to a new buffer holding `count` elements using the given allocator.
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
 * - Use only with types that are trivially copyable (copied via memcpy, destructors not invoked).
 */
template <typename T>
[[nodiscard]] Memory<T> memoryReallocate(core::Memory<T>&& mem, addr_size count, core::AllocatorId allocatorId) {
    static_assert(std::is_trivially_copyable_v<T>, "T should be trivially copyable");

    auto& actx = core::getAllocator(allocatorId);
    return memoryReallocate(std::move(mem), count, actx);
}
template <typename T>
[[nodiscard]] Memory<T> memoryReallocate(core::Memory<T>&& mem, addr_size count, core::AllocatorContext& actx) {
    static_assert(std::is_trivially_copyable_v<T>, "T should be trivially copyable");

    Assert(count >= mem.len(), "Trying to shrink with memoryMove is not advisable!");

    if (count == 0) return mem;

    core::Memory<T> ret;
    ret.ptr = reinterpret_cast<T*>(actx.zeroAlloc(count, sizeof(T)));
    ret.length = count;

    core::memcopy(ret.ptr, mem.data(),mem.len());

    memoryFree(std::move(mem), actx);

    return ret;
}

/**
 * \brief
 * Sets the element at index `i` to `v`, expanding the backing buffer as needed using `allocatorId`.
 *
 * If `i` is beyond the current length, the buffer is grown (doubling strategy) to fit `i`, reusing
 * `memoryReallocate` for allocation and copying.
 *
 * \param mem         The memory block to write into. Ownership may be transferred if reallocation occurs.
 * \param i           Index to set.
 * \param v           Value to write at index `i`.
 * \param allocatorId The allocator used when growing the buffer.
 *
 * \return A Memory<T> pointing to the (potentially reallocated) buffer with the value written.
 *
 * \warning
 * - The allocator must match the one that owns `mem`, otherwise behavior is undefined.
 * - Only trivially copyable types are supported.
 */
template <typename T>
[[nodiscard]] Memory<T> memorySet(core::Memory<T>& mem, addr_size i, T&& v, core::AllocatorId allocatorId) {
    static_assert(std::is_trivially_copyable_v<T>, "T should be trivially copyable");

    auto& actx = core::getAllocator(allocatorId);
    return memorySet(mem, i, std::move(v), actx);
}
template <typename T>
[[nodiscard]] Memory<T> memorySet(core::Memory<T>& mem, addr_size i, T&& v, core::AllocatorContext& actx) {
    static_assert(std::is_trivially_copyable_v<T>, "T should be trivially copyable");

    if (i >= mem.len()) {
        addr_size newLen = mem.len() == 0 ? 1 : mem.len();
        while (i >= newLen) {
            newLen *= 2;
        }

        mem = memoryReallocate(std::move(mem), newLen, actx);
    }

    mem[i] = std::move(v);
    return mem;
}

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
