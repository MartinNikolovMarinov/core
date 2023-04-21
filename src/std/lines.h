#pragma once

#include <types.h>
#include <std/vec.h>

#include <cmath>

namespace core {

using namespace coretypes;

template <typename TFloat>
inline TFloat slope(TFloat x1, TFloat y1, TFloat x2, TFloat y2) {
    TFloat raise = y2 - y1;
    TFloat run = x2 - x1;
    Assert(raise == 0 || run != 0, "division by zero");
    return raise / run;
}

template <typename TFloat>
inline TFloat slope_to_deg(TFloat x1, TFloat y1, TFloat x2, TFloat y2) {
    TFloat raise = y2 - y1;
    TFloat run = x2 - x1;
    return rad_to_deg(std::atan2(raise, run));
}

template <typename TFloat>
inline core::vec2<TFloat> line_intersection(TFloat x1, TFloat y1, TFloat x2, TFloat y2) {
    TFloat m = slope(x1, y1, x2, y2);
    TFloat b = y1 - m * x1;
    return core::vec2<TFloat>{-b / m, b};
}

} // namespace core
