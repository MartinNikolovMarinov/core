#pragma once

#include <core_API.h>
#include <core_types.h>
#include <core_traits.h>

namespace core {

using namespace coretypes;

template <typename T> addr_size hash(const T& key) = delete;
template <typename T> bool eq(const T& a, const T& b) = delete;

template <typename T>
concept HashableConcept = requires {
    { core::hash<T>(std::declval<const T&>()) } -> core::same_as<addr_size>;
    { core::eq<T>(std::declval<const T&>(), std::declval<const T&>()) } -> core::same_as<bool>;
};

CORE_API_EXPORT u64 fnv1a_64(const void* input, addr_size len, u64 seed = 0);

CORE_API_EXPORT u64 djb2_64(const void* input, addr_size len, u64 seed = 0);

} // namespace core
