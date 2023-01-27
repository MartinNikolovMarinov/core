#pragma once

#include <API.h>
#include <types.h>
#include <utils.h>

// TODO: Including new might break the nostdlib build on some systems?
#include <new>

namespace core {

using namespace coretypes;

/**
 * @brief The allocated memory is not guaranteed to be exactly "size" bytes, but it will be at least "size" bytes. This
 *        is because the allocator may need to align the memory.
*/
template<typename A>
void* CORE_API_EXPORT alloc(ptr_size size) noexcept {
    return A::alloc(size);
}

/**
 * @brief The implementation of construct should call the constructor of T with the given args.
*/
template<typename A, typename T, typename ...Args>
T* CORE_API_EXPORT construct(A& allocator, T&&, Args&&... args) noexcept {
    void* p = allocator.alloc(sizeof(T));
    return new (p) T(core::forward<Args>(args)...);
}

/**
 * @brief The implementation of used_mem should return the amount of memory used by the allocator.
 *        NOTE: The overall size may be larger than what the user has asked for, due to memory alignment.
*/
template<typename A>
ptr_size CORE_API_EXPORT used_mem() noexcept {
    return A::used_mem();
}

/**
 * @brief Frees allocated memory at the given address. Freeing nullptr is a no-op, but should not crash. Freeing and
 *        address that was not allocated by the allocator is undefined behavior.
 *        NOTE: Some allocation algorithms may not be able to free memory.
*/
template<typename A>
void CORE_API_EXPORT free(void* ptr) noexcept {
    A::free(ptr);
}

/**
 * @brief Return the name of the allocator.
*/
template<typename A>
const char* CORE_API_EXPORT allocator_name() noexcept {
    return A::allocator_name();
}

/**
 * @brief This macro should be overriden by the user to specify the default allocator to use.
*/
#define CORE_DEFAULT_ALLOCATOR() void

} // namespace core
