#pragma once

#include "init_core.h"

using namespace coretypes;

struct Grid2D {
    core::vec2f min;
    core::vec2f max;

    constexpr core::vec2f center() const {
        return (min + max) / 2;
    }

    constexpr core::vec2f convertTo(f32 x, f32 y, const Grid2D& to) const {
        return convertCommon(x, y, this->max, this->min, to.max, to.min);
    }

    constexpr core::vec2f convertTo_v(const core::vec2f& v, const Grid2D& to) const {
        return convertTo(v.x(), v.y(), to);
    }

    constexpr core::vec2f convertFrom(f32 x, f32 y, const Grid2D& from) const {
        return convertCommon(x, y, from.max, from.min, this->max, this->min);
    }

    constexpr core::vec2f convertFrom_v(const core::vec2f& v, const Grid2D& from) const {
        return convertFrom(v.x(), v.y(), from);
    }

    constexpr core::mat4x4f getToConvMatrix(const Grid2D& to) const {
        return convertCommon(this->max, this->min, to.max, to.min);
    }

    constexpr core::mat4x4f getFromConvMatrix(const Grid2D& from) const {
        return convertCommon(from.max, from.min, this->max, this->min);
    }

private:
    static constexpr core::vec2f convertCommon(f32 x, f32 y,
                                               const core::vec2f& fromMax, const core::vec2f& fromMin,
                                               const core::vec2f& toMax, const core::vec2f& toMin) {
        core::vec2f fromRange = fromMax - fromMin;
        core::vec2f toRange = toMax - toMin;
        core::vec2f relativeLoc = (core::v(x, y) - fromMin) / fromRange;
        core::vec2f ret = relativeLoc * toRange + toMin;
        return ret;
    }

    static constexpr core::mat4x4f convertCommon(const core::vec2f& fromMax, const core::vec2f& fromMin,
                                                 const core::vec2f& toMax, const core::vec2f& toMin) {
        core::mat4x4f m = core::mat4x4f::identity();
        m[0][0] = (toMax.x() - toMin.x()) / (fromMax.x() - fromMin.x());
        m[1][1] = (toMax.y() - toMin.y()) / (fromMax.y() - fromMin.y());
        m[0][3] = toMin.x() - m[0][0] * fromMin.x();
        m[1][3] = toMin.y() - m[1][1] * fromMin.y();
        return m;
    }
};
