#pragma once

#include <API.h>
#include <types.h>
#include <utils.h>

/**
 * This file declares the core memory allocation interface.
*/
namespace core::alloc
{

using namespace coretypes;

/**
 * @brief Implementing this interface allows generic programming over alloc::Alloc.
 *        The allocated memory is not guaranteed to be exactly size bytes, but it
 *        will be at least size bytes. This is because the allocator may need to
 *        align the memory.
 *
 * @param[in] allocator The allocator to allocate from
 * @param[in] size The size of the allocation
 * @return The address of the allocated memory
*/
template<typename A>
CORE_API_EXPORT void* Alloc(A& allocator, ptr_size size) noexcept {
    return allocator.Alloc(size);
}
template<typename A>
CORE_API_EXPORT void* Alloc(A&& allocator, ptr_size size) noexcept {
    return allocator.Alloc(size);
}

/**
 * @brief Implementing this interface allows generic programming over alloc::Construct.
 *        The implementation of Construct should call the constructor of T with the
 *        given args.
 *
 * @param[in] allocator The allocator to construct with
 * @param[in] out The address to construct at. This is needed for type deduction
 * @param[in] args The arguments to pass to the constructor
 * @return A pointer to the constructed object
*/
template<typename A, typename T, typename ...Args>
CORE_API_EXPORT T* Construct(A& allocator, T* out, Args&&... args) noexcept {
    return allocator.Construct(out, core::Forward<Args>(args)...);
}
template<typename A, typename T, typename ...Args>
CORE_API_EXPORT T* Construct(A&& allocator, T* out, Args&&... args) noexcept {
    return allocator.Construct(out, core::Forward<Args>(args)...);
}

/**
 * @brief Implementing this interface allows generic programming over alloc::Free.
 *        The implementation of Free should free the memory at the given address.
 *        NOTE: that some allocation algorithms may not be able to free memory.
 *
 * @param[in] allocator The allocator to free with
 * @param[in] ptr The address to free
*/
template<typename A>
CORE_API_EXPORT void Free(A& allocator, void* addr) noexcept {
    allocator.Free(addr);
}
template<typename A>
CORE_API_EXPORT void Free(A&& allocator, void* addr) noexcept {
    allocator.Free(addr);
}

/**
 * @brief Implementing this interface allows generic programming over alloc::Clear.
 *        The implementation of Clear should free all memory allocated by the
 *        allocator.
 *        NOTE: that some allocation algorithms may not be able to clear all the memory.
 *
 * @param[in] allocator The allocator to clear
*/
template<typename A>
CORE_API_EXPORT void Clear(A& allocator) noexcept {
    allocator.Clear();
}
template<typename A>
CORE_API_EXPORT void Clear(A&& allocator) noexcept {
    allocator.Clear();
}

/**
 * @brief Implementing this interface allows generic programming over alloc::UsedMem.
 *        The implementation of UsedMem should return the amount of memory used by
 *        the allocator.
 *        NOTE: that the overall size may be larger than what the user has asked for,
 *        due to memory alignment.
 *
 * @param[in] allocator The allocator to get the used memory of
 * @return The amount of memory used by the allocator
*/
template<typename A>
CORE_API_EXPORT ptr_size UsedMem(A& allocator) noexcept {
    return allocator.UsedMem();
}
template<typename A>
CORE_API_EXPORT ptr_size UsedMem(A&& allocator) noexcept {
    return allocator.UsedMem();
}

// Customizeble global assert handler:
CORE_API_EXPORT void SetGlobalAssertHandler(GlobalAssertHandlerPtr handler);
CORE_API_EXPORT GlobalAssertHandlerPtr GetGlobalAssertHandler();


} // namespace core::alloc
