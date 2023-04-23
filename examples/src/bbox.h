#pragma once

#include "init_core.h"

using namespace coretypes;

struct Bbox2D {

    constexpr Bbox2D() : topLeft({}), bottomRight({}) {}
    constexpr Bbox2D(const core::vec2f& topLeft, const core::vec2f& bottomRight) : topLeft(topLeft), bottomRight(bottomRight) {}

    constexpr core::vec2f center() const {
        return (topLeft + bottomRight) / 2;
    }

    constexpr void center(f32& x, f32 &y) {
        x = (topLeft.x() + bottomRight.x()) / 2;
        y = (topLeft.y() + bottomRight.y()) / 2;
    }

    constexpr f32 width() const {
        return bottomRight.x() - topLeft.x();
    }

    constexpr f32 height() const {
        return bottomRight.y() - topLeft.y();
    }

    constexpr bool isInside(f32 x, f32 y) const {
        return x >= topLeft.x() && x <= bottomRight.x() && y >= topLeft.y() && y <= bottomRight.y();
    }

    core::vec2f topLeft;
    core::vec2f bottomRight;
};
