#pragma once

#include <cmath>

#include <API.h>
#include <types.h>
#include <core_math.h>

namespace core {

#pragma region Round -------------------------------------------------------------------------------------------------

constexpr f32 round(f32 x) {
    if constexpr (core::is_const_evaluated()) {
        Assert("TODO: Not implemented yet.");
    }
    return std::round(x);
}

// Musl implementation of round.
constexpr f64 round(f64 x) {
    if constexpr (core::is_const_evaluated()) {
        Assert("TODO: Not implemented yet.");
    }
    return std::round(x);
}

template <typename TFloat>
constexpr TFloat round_to(TFloat n, u32 to) {
    return TFloat(round(n * TFloat(10*to))) / TFloat(10*to);
}

#pragma endregion

#pragma region Floor -------------------------------------------------------------------------------------------------

constexpr f32 floor(f32 x) {
    if constexpr (core::is_const_evaluated()) {
        Assert("TODO: Not implemented yet.");
    }
    return std::floor(x);
}

constexpr f64 floor(f64 x) {
    if constexpr (core::is_const_evaluated()) {
        Assert("TODO: Not implemented yet.");
    }
    return std::floor(x);
}

#pragma endregion

#pragma region Ceil --------------------------------------------------------------------------------------------------

constexpr f32 ceil(f32 x) {
    if constexpr (core::is_const_evaluated()) {
        Assert("TODO: Not implemented yet.");
    }
    return std::ceil(x);
}

constexpr f64 ceil(f64 x) {
    if constexpr (core::is_const_evaluated()) {
        Assert("TODO: Not implemented yet.");
    }
    return std::ceil(x);
}

#pragma endregion

#pragma region Sqrt --------------------------------------------------------------------------------------------------

constexpr f32 sqrt(f32 x) {
    if constexpr (core::is_const_evaluated()) {
        Assert("TODO: Not implemented yet.");
    }
    return std::sqrt(x);
}

constexpr f64 sqrt(f64 x) {
    if constexpr (core::is_const_evaluated()) {
        Assert("TODO: Not implemented yet.");
    }
    return std::sqrt(x);
}

#pragma endregion

} // namespace core
