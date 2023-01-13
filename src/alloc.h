#pragma once

#include <API.h>
#include <types.h>
#include <utils.h>

namespace core {

using namespace coretypes;

/**
 * @brief The allocated memory is not guaranteed to be exactly "size" bytes, but it will be at least "size" bytes. This
 *        is because the allocator may need to align the memory.
 *
 * @param[in] allocator The allocator to allocate from
 * @param[in] size The size of the allocation
 * @return The address of the allocated memory
*/
template<typename A>
void* CORE_API_EXPORT alloc(A& allocator, ptr_size size) noexcept {
    return allocator.alloc(size);
}
template<typename A>
void* CORE_API_EXPORT alloc(A&& allocator, ptr_size size) noexcept {
    return allocator.alloc(size);
}

/**
 * @brief The implementation of UsedMem should return the amount of memory used by the allocator.
 *        NOTE: The overall size may be larger than what the user has asked for, due to memory alignment.
 *
 * @param[in] allocator The allocator to get the used memory of
 * @return The amount of memory used by the allocator
*/
template<typename A>
ptr_size CORE_API_EXPORT used_mem(A& allocator) noexcept {
    return allocator.used_mem();
}
template<typename A>
ptr_size CORE_API_EXPORT used_mem(A&& allocator) noexcept {
    return allocator.used_mem();
}

/**
 * @brief The implementation of Construct should call the constructor of T with the given args.
 *
 * @param[in] allocator The allocator to construct with
 * @param[in] out The address to construct at. This is needed for type deduction
 * @param[in] args The arguments to pass to the constructor
 * @return A pointer to the constructed object
*/
template<typename A, typename T, typename ...Args>
T* CORE_API_EXPORT construct(A& allocator, T* out, Args&&... args) noexcept {
    return allocator.construct(out, core::forward<Args>(args)...);
}
template<typename A, typename T, typename ...Args>
T* CORE_API_EXPORT construct(A&& allocator, T* out, Args&&... args) noexcept {
    return allocator.construct(out, core::forward<Args>(args)...);
}

/**
 * @brief Frees allocated memory at the given address. Freeing nullptr is a no-op, but should not crash. Freeing and
 *        address that was not allocated by the allocator is undefined behavior.
 *        NOTE: Some allocation algorithms may not be able to free memory.
 *
 * @param[in] allocator The allocator to free with
 * @param[in] ptr The address to free
*/
template<typename A>
void CORE_API_EXPORT free(A& allocator, void* ptr) noexcept {
    allocator.free(ptr);
}
template<typename A>
void CORE_API_EXPORT free(A&& allocator, void* ptr) noexcept {
    allocator.free(ptr);
}

// IMPORTANT: These 2 functions need to be implemented by the user of the library:
using allocator_id = i32;
constexpr allocator_id default_allocator_id();
constexpr auto get_allocator(allocator_id id);

} // namespace core
