#pragma once

#include <core_types.h>

#include <math/core_math.h>
#include <math/core_bbox.h>

namespace core {

using namespace coretypes;

template<typename T>
f32 calcTriangleAreaF32(T ax, T ay, T bx, T by, T cx, T cy) {
    static_assert(std::is_integral_v<T>, "T must be integral type");
    f32 ret =  0.5f * f32((by-ay)*(bx+ax) + (cy-by)*(cx+bx) + (ay-cy)*(ax+cx)); // Shoelace formula
    return ret;
}

template<typename T>
f64 calcTriangleAreaF64(T ax, T ay, T bx, T by, T cx, T cy) {
    static_assert(std::is_integral_v<T>, "T must be integral type");
    f64 ret =  0.5 * f64((by-ay)*(bx+ax) + (cy-by)*(cx+bx) + (ay-cy)*(ax+cx)); // Shoelace formula
    return ret;
}

template<typename T>
Bbox2D<T> calcTriangleBBox(T ax, T ay, T bx, T by, T cx, T cy) {
    static_assert(std::is_integral_v<T>, "T must be integral type");

    T minx = core::core_min(core::core_min(ax, bx), cx);
    T miny = core::core_min(core::core_min(ay, by), cy);
    T maxx = core::core_max(core::core_max(ax, bx), cx);
    T maxy = core::core_max(core::core_max(ay, by), cy);

    Bbox2D bbox (core::v(minx, miny), core::v(maxx, maxy));
    return bbox;
}

} // namespace core
