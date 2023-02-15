#pragma once

#include "init_core.h"

using namespace coretypes;

struct Grid2D {
    core::vec2f min;
    core::vec2f max;

    constexpr core::vec2f center() {
        return (min + max) / 2;
    }
};

constexpr core::vec2f convertVecUsingGrid(const core::vec2f& src, const Grid2D& from, const Grid2D& to) {
    core::vec2f fromRange = from.max - from.min;
    core::vec2f toRange = to.max - to.min;
    core::vec2f relativeLoc = (src - from.min) / fromRange;
    core::vec2f ret = relativeLoc * toRange + to.min;
    return ret;
}
