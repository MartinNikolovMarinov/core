#pragma once

#include "init_core.h"

using namespace coretypes;

struct Shape2D {
    struct Usage {
        enum Access : u32 {
            STATIC = 0,
            DYNAMIC = 1,
            STREAM = 2,
        };
        enum AccessType : u32 {
            DRAW = 0,
            READ = 1,
            COPY = 2,
        };

        Access access;
        AccessType atype;

        u32 usageGL() const;
    };

    struct RenderMode {
        enum Mode : u32 {
            POINTS = 0,
            LINES = 1,
            LINE_STRIP = 2,
            LINE_LOOP = 3,
            TRIANGLES = 4,
            TRIANGLE_STRIP = 5,
            TRIANGLE_FAN = 6,
        };

        Mode mode;

        u32 modeGL() const;
    };

    struct VertexLayout {
        u32 stride;
        u32 offset;
        Usage usage;

        // TODO: below this point are parameters that are not really vertex layout... anyway fix this later.
        //       Are they leaky abstractions? Should they even be in this struct?
        i32 posAttribId;
        RenderMode renderMode;
    };

    Shape2D() = default;
    ~Shape2D();

    static Shape2D create(const Shape2D::VertexLayout& vl,
                          const core::vec4u& colorRGB,
                          f32 zIndex,
                          core::arr<core::vec2f>&& vertices);

    static Shape2D createRect2D(const Shape2D::VertexLayout& vl,
                                core::vec2f pos, f32 width, f32 height,
                                const core::vec4u& colorRGB,
                                f32 zIdx = 0.0f);

    static Shape2D createTriangle2D(const Shape2D::VertexLayout& vl,
                         core::vec2f a, core::vec2f b, core::vec2f c,
                         const core::vec4u& colorRGB,
                         f32 zIdx = 0.0f);

    static Shape2D createCircle2D(const Shape2D::VertexLayout& vl,
                                  core::vec2f center, f32 radius, u32 steps,
                                  const core::vec4u& colorRGB, f32 zIdx = 0.0f);

    u32 vboID() const;
    u32 vaoID() const;
    f32 zIdx() const;
    const core::arr<core::vec2f>& vertices() const;
    i32 vertexCount() const;
    core::vec4u colorRGB() const;
    const core::vec4f& color() const;
    const RenderMode& renderMode() const;

    void destroy() const;

private:
    // TODO: Do I actually need to store indices in the shape when I will only render in 2d?
    //       I think I do. Because rendering line strips, circles and some more complex objects
    //       will duplicate a lot of vertices. Leaving it for later.
    core::arr<core::vec2f> m_vertices;
    core::vec4f m_color;
    RenderMode m_renderMode;
    u32 m_vboId;
    u32 m_vaoId;
    f32 m_zIndex;
};
