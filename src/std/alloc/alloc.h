#pragma once

#include <API.h>
#include <types.h>

#include <utility>

/**
 * This file declares the core memory allocation interface.
*/
namespace core::alloc
{

using namespace coretypes;

/**
 * @brief TODO: comment
*/
template<typename A> CORE_API_EXPORT auto Alloc(A& allocator, ptr_size size) noexcept -> decltype(allocator.Alloc(size)) {
    return allocator.Alloc(size);
}

/**
 * @brief TODO: comment
*/
template<typename A, typename T, typename ...Args> CORE_API_EXPORT T* Construct(A& allocator, T* out, Args&&... args) noexcept {
    return allocator.Construct(out, std::forward<Args>(args)...);
}

/**
 * @brief TODO: comment
*/
template<typename A> CORE_API_EXPORT void Free(A& allocator, void* addr) noexcept {
    allocator.Free(addr);
}

/**
 * @brief TODO: comment
*/
template<typename A> CORE_API_EXPORT void Clear(A& allocator) noexcept {
    allocator.Clear();
}

/**
 * @brief TODO: comment
*/
template<typename A> CORE_API_EXPORT ptr_size UsedMem(A& allocator) noexcept {
    return allocator.UsedMem();
}

} // namespace core::alloc
