#pragma once

#include "init_core.h"

using namespace coretypes;

struct Bbox2D {

    constexpr Bbox2D() : min({}), max({}) {}
    constexpr Bbox2D(const core::vec2f& min, const core::vec2f& max) : min(min), max(max) {
        // It's good to assume
        Assert(this->min.x() < this->max.x(), "Invalid bbox!");
        Assert(this->min.y() < this->max.y(), "Invalid bbox!");
    }

    constexpr core::vec2f center() const {
        return (min + max) / 2;
    }

    constexpr void center(f32& x, f32 &y) {
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

    constexpr bool isInside(f32 x, f32 y) const {
        return x >= min.x() && x <= max.x() && y >= min.y() && y <= max.y();
    }

    struct IntersectionResult {
        core::vec2f intersections[2];
        bool hasEntry, hasExit;
        u32 intersectionCount;
    };

    // IMPORTANT: This code assumes y increses from top to bottom.
    // TODO: Perhaps this could be implemented faster without using lineToLineIntersection.
    //       Also the high epsilon value sticks out like a soar thumb.
    constexpr IntersectionResult intersection(const core::vec2f& lp0, const core::vec2f& lp1) const {
        constexpr f32 epsilon = 0.0001f;
        IntersectionResult res = {};
        core::vec2f t, entry, exit;
        const f32 dx = lp1.x() - lp0.x();
        const f32 dy = lp1.y() - lp0.y();

        core::vec2f p0 = min; // top left
        core::vec2f p1 = core::v(max.x(), min.y()); // top right
        core::vec2f p2 = core::v(min.x(), max.y()); // bottom left
        core::vec2f p3 = max; // bottom right

        auto intersectionWithBoxLineSegment = [&, this](const core::vec2f& p0, const core::vec2f& p1, core::vec2f& t) -> bool {
            core::lineToLineIntersection(lp0, lp1, p0, p1, t);
            bool ret = min.x() - t.x() <= epsilon &&
                       t.x() - max.x() <= epsilon &&
                       min.y() - t.y() <= epsilon &&
                       t.y() - max.y() <= epsilon;
            return ret;
        };

        if (dy > 0) {
            if (p1.y() >= lp0.y() && p1.y() <= lp1.y() && intersectionWithBoxLineSegment(p0, p1, t)) {
                // Intersection with top line.
                entry = t;
                res.hasEntry = true;
            }
            if (p2.y() >= lp0.y() && p2.y() <= lp1.y() && intersectionWithBoxLineSegment(p2, p3, t)) {
                // Intersection with bottom line.
                exit = t;
                res.hasExit = true;
            }
        }
        else {
            if (p1.y() <= lp0.y() && p1.y() >= lp1.y() && intersectionWithBoxLineSegment(p0, p1, t)) {
                // Intersection with top line.
                exit = t;
                res.hasExit = true;
            }
            if (p2.y() <= lp0.y() && p2.y() >= lp1.y() && intersectionWithBoxLineSegment(p2, p3, t)) {
                // Intersection with bottom line.
                entry = t;
                res.hasEntry = true;
            }
        }

        if (dx > 0) {
            if (p3.x() >= lp0.x() && p3.x() <= lp1.x() && intersectionWithBoxLineSegment(p1, p3, t)) {
                // Intersection with right line.
                exit = t;
                res.hasExit = true;
            }
            if (p0.x() >= lp0.x() && p0.x() <= lp1.x() && intersectionWithBoxLineSegment(p0, p2, t)) {
                // Intersection with left line.
                entry = t;
                res.hasEntry = true;
            }
        }
        else {
            if (p3.x() <= lp0.x() && p3.x() >= lp1.x() && intersectionWithBoxLineSegment(p1, p3, t)) {
                // Intersection with right line.
                entry = t;
                res.hasEntry = true;
            }
            if (p0.x() <= lp0.x() && p0.x() >= lp1.x() && intersectionWithBoxLineSegment(p0, p2, t)) {
                // Intersection with left line.
                exit = t;
                res.hasExit = true;
            }
        }

        if (res.hasEntry) {
            res.intersections[0] = entry;
            res.intersectionCount++;
        }
        if (res.hasExit) {
            res.intersections[1] = exit;
            res.intersectionCount++;
        }

        return res;
    }

    core::vec2f interpolate(const Bbox2D& other, const core::vec2f& t) {
        core::vec2f from = max - min;
        core::vec2f to = other.max - other.min;
        core::vec2f res = from + t * (to - from);
        return res;
    }

    core::vec2f min;
    core::vec2f max;
};
