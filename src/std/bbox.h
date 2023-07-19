#pragma once

#include <types.h>
#include <core_math.h>
#include <mem.h>
#include <std/vec.h>
#include <std/mat.h>
#include <std/lines.h>

namespace core {

using namespace coretypes;

struct bbox_2d {

    constexpr bbox_2d() : min({}), max({}) {}
    constexpr bbox_2d(const core::vec2f& min, const core::vec2f& max) : min(min), max(max) {}

    constexpr core::vec2f center() const { return (min + max) / 2; }

    constexpr void center(f32& x, f32 &y) const {
        core::vec2f c = center();
        x = c.x();
        y = c.y();
    }

    constexpr f32 width() const {
        return max.x() - min.x();
    }

    constexpr f32 height() const {
        return max.y() - min.y();
    }

    constexpr bool is_inside(f32 x, f32 y) const {
        return x >= min.x() && x <= max.x() && y >= min.y() && y <= max.y();
    }

    constexpr bool is_inside(const core::vec2f& x) const {
        return is_inside(x.x(), x.y());
    }

    constexpr core::vec2f conv_to(const core::vec2f& v, const bbox_2d& to) const {
        core::vec2f ret = core::affine_map(v, min, max, to.min, to.max);
        return ret;
    }

    struct intersection_result {
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
     * @return intersection_result The intersection result.
    */
    constexpr intersection_result intersection_with_line(const core::vec2f& start, const core::vec2f& end) const {
        intersection_result res;

        const auto dir = end - start;

        if (dir.x() == 0 && dir.y() == 0) {
            // Not moving - no intersection
            res.hasEntry = false;
            res.hasExit = false;
            return res;
        }

        f32 tMin = core::MIN_F32;
        f32 tMax = core::MAX_F32;

        if (dir.x() != 0) {
            const f32 invDirX = 1.0f / dir.x();
            f32 tMinX = (min.x() - start.x()) * invDirX;
            f32 tMaxX = (max.x() - start.x()) * invDirX;
            if (tMinX > tMaxX) core::swap(tMinX, tMaxX);
            tMin = core::max(tMin, tMinX);
            tMax = core::min(tMax, tMaxX);
        }

        if (dir.y() != 0) {
            const f32 invDirY = 1.0f / dir.y();
            f32 tMinY = (min.y() - start.y()) * invDirY;
            f32 tMaxY = (max.y() - start.y()) * invDirY;
            if (tMinY > tMaxY) core::swap(tMinY, tMaxY);
            tMin = core::max(tMin, tMinY);
            tMax = core::min(tMax, tMaxY);
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
     *        This is slightly slower than intersection_with_line, but provides exact entry and exit points.
     *
     * @param start The start of the line segment.
     * @param end The end of the line segment.
     * @return intersection_result The intersection result.
    */
    constexpr intersection_result intersection_with_line_segment(const core::vec2f& start, const core::vec2f& end) const {
        intersection_result res = {};
        core::vec2f ipoint; // intersection point
        const f32 dx = end.x() - start.x();
        const f32 dy = end.y() - start.y();

        core::vec2f p0 = min; // top left
        core::vec2f p1 = core::v(max.x(), min.y()); // top right
        core::vec2f p2 = core::v(min.x(), max.y()); // bottom left
        core::vec2f p3 = max; // bottom right

        auto intersectionWithBoxLineSegment = [&, this](const core::vec2f& a, const core::vec2f& b, core::vec2f& p) -> bool {
            constexpr f32 epsilon = 0.0001f;
            core::line_to_line_intersection(start, end, a, b, p);
            bool ret = min.x() - p.x() <= epsilon &&
                       p.x() - max.x() <= epsilon &&
                       min.y() - p.y() <= epsilon &&
                       p.y() - max.y() <= epsilon;
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

    core::vec2f min;
    core::vec2f max;
};

}

