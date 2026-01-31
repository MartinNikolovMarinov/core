#pragma once

#include <core_mem.h>
#include <core_types.h>
#include <math/core_lines.h>
#include <math/core_math.h>
#include <math/core_matrix.h>
#include <math/core_vec.h>

namespace core {

using namespace coretypes;

template<typename T>
struct Bbox2D {

    constexpr Bbox2D() : min({}), max({}) {}
    constexpr Bbox2D(const core::vec2<T>& _min, const core::vec2<T>& _max) : min(_min), max(_max) {}

    constexpr core::vec2<T> center() const { return (min + max) / 2; }

    constexpr void center(f32& x, f32 &y) const {
        core::vec2<T> c = center();
        x = c.x();
        y = c.y();
    }

    constexpr f32 width() const {
        return max.x() - min.x();
    }

    constexpr f32 height() const {
        return max.y() - min.y();
    }

    constexpr bool isInside(f32 x, f32 y) const {
        return x >= min.x() && x <= max.x() && y >= min.y() && y <= max.y();
    }

    constexpr bool isInside(const core::vec2<T>& x) const {
        return isInside(x.x(), x.y());
    }

    constexpr core::vec2<T> convTo(const core::vec2<T>& v, const Bbox2D& to) const {
        core::vec2<T> ret = core::affineMap(v, min, max, to.min, to.max);
        return ret;
    }

    constexpr void clampTo(T minx, T maxx, T miny, T maxy) {
        min.x() = core::clamp(min.x(), minx, maxx);
        min.y() = core::clamp(min.y(), miny, maxy);
        max.x() = core::clamp(max.x(), minx, maxx);
        max.y() = core::clamp(max.y(), miny, maxy);
    }

    core::vec2<T> min;
    core::vec2<T> max;
};

// TODO2: [FIND_BETTER_PLACE_FOR_THIS_CODE] The right place for this type of code is probably in some raytracing helper functions.

struct BBoxIntersectionResult {
    core::vec2f entry;
    core::vec2f exit;
    bool hasEntry = false;
    bool hasExit = false;
};

/**
    * @brief Returns the intersection of the line [start, end] with the bounding box. NOT the line segment!
    *        The returned values are POTENTIAL intersection points along the line.
    *
    * @param start The start of the line.
    * @param end The end of the line.
    * @return IntersectionResult The intersection result.
*/
constexpr BBoxIntersectionResult bboxIntersectionWithLine(const Bbox2D<f32>& bbox, const core::vec2f& start, const core::vec2f& end) {
    BBoxIntersectionResult res;

    const auto dir = end - start;

    if (dir.x() == 0 && dir.y() == 0) {
        // Not moving - no intersection
        res.hasEntry = false;
        res.hasExit = false;
        return res;
    }

    f32 tMin = core::limitMin<f32>();
    f32 tMax = core::limitMax<f32>();

    if (dir.x() != 0) {
        const f32 invDirX = 1.0f / dir.x();
        f32 tMinX = (bbox.min.x() - start.x()) * invDirX;
        f32 tMaxX = (bbox.max.x() - start.x()) * invDirX;
        if (tMinX > tMaxX) core::swap(tMinX, tMaxX);
        tMin = core::core_max(tMin, tMinX);
        tMax = core::core_min(tMax, tMaxX);
    }

    if (dir.y() != 0) {
        const f32 invDirY = 1.0f / dir.y();
        f32 tMinY = (bbox.min.y() - start.y()) * invDirY;
        f32 tMaxY = (bbox.max.y() - start.y()) * invDirY;
        if (tMinY > tMaxY) core::swap(tMinY, tMaxY);
        tMin = core::core_max(tMin, tMinY);
        tMax = core::core_min(tMax, tMaxY);
    }

    if (tMin >= tMax) {
        // No intersection
        res.hasEntry = false;
        res.hasExit = false;
        return res;
    }

    res.entry = start + tMin * dir;
    res.exit = start + tMax * dir;
    res.hasEntry = true;
    res.hasExit = true;

    return res;
}

/**
    * @brief Returns the intersection of the line segment [lp0, lp1] with the bounding box.
    *        This is slightly slower than intersectionWithLine, but provides exact entry and exit points.
    *
    * @param start The start of the line segment.
    * @param end The end of the line segment.
    * @return IntersectionResult The intersection result.
*/
constexpr BBoxIntersectionResult bboxIntersectionWithLineSegment(const Bbox2D<f32> bbox, const core::vec2f& start, const core::vec2f& end) {
    BBoxIntersectionResult res = {};
    core::vec2f ipoint; // intersection point
    const f32 dx = end.x() - start.x();
    const f32 dy = end.y() - start.y();

    core::vec2f p0 = bbox.min; // top left
    core::vec2f p1 = core::v(bbox.max.x(), bbox.min.y()); // top right
    core::vec2f p2 = core::v(bbox.min.x(), bbox.max.y()); // bottom left
    core::vec2f p3 = bbox.max; // bottom right

    auto intersectionWithBoxLineSegment = [&](const core::vec2f& a, const core::vec2f& b, core::vec2f& p) -> bool {
        constexpr f32 epsilon = 0.0001f;
        core::lineToLineIntersection(start, end, a, b, p);
        bool ret = bbox.min.x() - p.x() <= epsilon &&
                    p.x() - bbox.max.x() <= epsilon &&
                    bbox.min.y() - p.y() <= epsilon &&
                    p.y() - bbox.max.y() <= epsilon;
        return ret;
    };

    if (dy > 0) {
        if (p1.y() >= start.y() && p1.y() <= end.y() && intersectionWithBoxLineSegment(p0, p1, ipoint)) {
            // Intersection with top line.
            res.entry = ipoint;
            res.hasEntry = true;
        }
        if (p2.y() >= start.y() && p2.y() <= end.y() && intersectionWithBoxLineSegment(p2, p3, ipoint)) {
            // Intersection with bottom line.
            res.exit = ipoint;
            res.hasExit = true;
        }
    }
    else {
        if (p1.y() <= start.y() && p1.y() >= end.y() && intersectionWithBoxLineSegment(p0, p1, ipoint)) {
            // Intersection with top line.
            res.exit = ipoint;
            res.hasExit = true;
        }
        if (p2.y() <= start.y() && p2.y() >= end.y() && intersectionWithBoxLineSegment(p2, p3, ipoint)) {
            // Intersection with bottom line.
            res.entry = ipoint;
            res.hasEntry = true;
        }
    }

    if (dx > 0) {
        if (p3.x() >= start.x() && p3.x() <= end.x() && intersectionWithBoxLineSegment(p1, p3, ipoint)) {
            // Intersection with right line.
            res.exit = ipoint;
            res.hasExit = true;
        }
        if (p0.x() >= start.x() && p0.x() <= end.x() && intersectionWithBoxLineSegment(p0, p2, ipoint)) {
            // Intersection with left line.
            res.entry = ipoint;
            res.hasEntry = true;
        }
    }
    else {
        if (p3.x() <= start.x() && p3.x() >= end.x() && intersectionWithBoxLineSegment(p1, p3, ipoint)) {
            // Intersection with right line.
            res.entry = ipoint;
            res.hasEntry = true;
        }
        if (p0.x() <= start.x() && p0.x() >= end.x() && intersectionWithBoxLineSegment(p0, p2, ipoint)) {
            // Intersection with left line.
            res.exit = ipoint;
            res.hasExit = true;
        }
    }

    return res;
}

}
