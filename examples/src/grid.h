#pragma once

#include "init_core.h"

using namespace coretypes;

struct Grid2D {
    core::vec2f min;
    core::vec2f max;

    constexpr Grid2D(const core::vec2f& min, const core::vec2f& max) : min(min), max(max) {
        Assert(min.x() < max.x());
        Assert(min.y() < max.y());
    }

    constexpr core::vec2f center() const {
        return (min + max) / 2;
    }

    constexpr core::vec2f convertTo(f32 x, f32 y, const Grid2D& to) const {
        core::vec2f fromRange = this->max - this->min;
        core::vec2f toRange = to.max - to.min;
        core::vec2f relativeLoc = (core::v(x, y) - this->min) / fromRange;
        core::vec2f ret = relativeLoc * toRange + to.min;
        return ret;
    }

    constexpr core::vec2f convertTo_v(const core::vec2f& v, const Grid2D& to) const {
        return convertTo(v.x(), v.y(), to);
    }

    constexpr core::vec2f convertFrom(f32 x, f32 y, const Grid2D& from) const {
        core::vec2f fromRange = from.max - from.min;
        core::vec2f toRange = this->max - this->min;
        core::vec2f relativeLoc = (core::v(x, y) - from.min) / fromRange;
        core::vec2f ret = relativeLoc * toRange + this->min;
        return ret;
    }

    constexpr core::vec2f convertFrom_v(const core::vec2f& v, const Grid2D& from) const {
        return convertFrom(v.x(), v.y(), from);
    }

    constexpr core::mat4x4f getToConvMatrix(const Grid2D& to) const {
        core::mat4x4f m = core::mat4x4f::identity();
        m[0][0] = (to.max.x() - to.min.x()) / (this->max.x() - this->min.x());
        m[1][1] = (to.max.y() - to.min.y()) / (this->max.y() - this->min.y());
        m[0][3] = to.min.x() - m[0][0] * this->min.x();
        m[1][3] = to.min.y() - m[1][1] * this->min.y();
        return m;
    }

    constexpr core::mat4x4f getFromConvMatrix(const Grid2D& from) const {
        core::mat4x4f m = core::mat4x4f::identity();
        m[0][0] = (this->max.x() - this->min.x()) / (from.max.x() - from.min.x());
        m[1][1] = (this->max.y() - this->min.y()) / (from.max.y() - from.min.y());
        m[0][3] = this->min.x() - m[0][0] * from.min.x();
        m[1][3] = this->min.y() - m[1][1] * from.min.y();
        return m;
    }
};
