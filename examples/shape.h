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

        Access a;
        AccessType t;

        u32 usage_Gl() const;
    };

    struct VertexLayout {
        u32 stride;
        u32 offset;
        i32 posAttribId;
        Usage usage;
    };

    static Shape2D create(const Shape2D::VertexLayout& vl,
                          const core::vec4f& fillColor,
                          f32 zIndex,
                          core::arr<core::vec2f>&& vertices);

    u32 vbo_id() const;
    u32 vao_id() const;
    f32 z_index() const;
    const core::arr<core::vec2f>& vertices() const;
    i32 vertex_count() const;
    const core::vec4f& fill_color() const;

    void destroy() const;

private:
    // TODO: Do I actually need to store indices in the shape when I will only render in 2d?
    //       I think I do. Because rendering line strips, circles and some more complex objects
    //       will duplicate a lot of vertices. Leaving it for later.
    core::arr<core::vec2f> m_vertices;
    core::vec4f m_fillColor;
    u32 m_vboId;
    u32 m_vaoId;
    f32 m_zIndex;
};
