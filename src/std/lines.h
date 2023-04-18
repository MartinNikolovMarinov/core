#pragma once

#include <types.h>

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

} // namespace core
