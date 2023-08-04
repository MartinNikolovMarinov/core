#pragma once

#include <types.h>
#include <utils.h>
#include <compiler.h>
#include <new>

namespace core {

using namespace coretypes;

/**
 * @brief The allocated memory is not guaranteed to be exactly "size" bytes, but it will be at least "size" bytes. This
 *        is because the allocator may need to align the memory.
*/
template<typename A>
void* alloc(ptr_size size) noexcept {
    return A::alloc(size);
}

/**
 * @brief Uses the allocator to allocate memory and calls the constructor of T with the given args.
*/
template<typename A, typename T, typename ...Args>
T* construct(A& allocator, T&&, Args&&... args) noexcept {
    void* p = allocator.alloc(sizeof(T));
    return new (p) T(core::forward<Args>(args)...);
}

/**
 * @brief The implementation of used_mem should return the amount of memory used by the allocator. Some allocators may
 *        be unable to return this information. The overall size may be larger than what the user has asked for, due to
 *        memory alignment.
*/
template<typename A>
ptr_size used_mem() noexcept {
    return A::used_mem();
}

/**
 * @brief Frees allocated memory at the given address. Freeing nullptr is a no-op, but should not crash. Freeing an
 *        address that was not allocated by the allocator is undefined behavior. Some allocation algorithms may not be
 *        able to free memory.
*/
template<typename A>
void free(void* ptr) noexcept {
    A::free(ptr);
}

/**
 * @brief Return the name of the allocator.
*/
template<typename A>
const char* allocator_name() noexcept {
    return A::allocator_name();
}

/**
 * @brief This macro should be overriden by the user to specify the default allocator. If it is not overriden, the
 *        DEFAULT template type for most data structures will fail to compile.
*/
#ifndef CORE_DEFAULT_ALLOCATOR
    #define CORE_DEFAULT_ALLOCATOR() void
#endif

} // namespace core
