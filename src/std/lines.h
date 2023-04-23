#pragma once

#include <types.h>
#include <std/vec.h>
#include <core_math.h>

#include <cmath>

namespace core {

using namespace coretypes;

template <typename TFloat>
inline TFloat slope(TFloat x1, TFloat y1, TFloat x2, TFloat y2) {
    TFloat raise = y2 - y1;
    TFloat run = x2 - x1;
    auto ret =  std::atan2(raise, run);
    return ret;
}

template <typename TFloat>
inline TFloat slope(const vec2<TFloat>& a, const vec2<TFloat>& b) {
    return slope(a.x(), a.y(), b.x(), b.y());
}

} // namespace core
