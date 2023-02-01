#pragma once

#include "init_core.h"

using namespace coretypes;

struct Grid2D {
    core::vec2i min;
    core::vec2i max;

    constexpr core::vec2i center() {
        return (min + max) / 2;
    }
};

constexpr core::vec2f convert_vec_using_grid(const core::vec2f& src, const Grid2D& from, const Grid2D& to) {
    core::vec2f fromRange = core::v_conv<core::vec2f>(from.max - from.min);
    core::vec2f toRange = core::v_conv<core::vec2f>(to.max - to.min);
    core::vec2f relativeLoc = (src - core::v_conv<core::vec2f>(from.min)) / fromRange;
    core::vec2f ret = relativeLoc * toRange + core::v_conv<core::vec2f>(to.min);
    return ret;
}
