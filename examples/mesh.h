#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <core.h>
#include <std/core.h>

using namespace coretypes;

struct Mesh2D {
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
        Usage usage;
    };

    static Mesh2D create(const Mesh2D::VertexLayout& vl, core::arr<core::vec2f>&& vertices);

    void destroy() const;

    i32 vertex_count() const;

    void bind() const;

private:
    // TODO: Do I actually need to store indices in the mesh when I will only render in 2d?
    //       Is this struct a mesh if it has not indices or normals? Kinda looks like a vertex buffer.
    //       Should I even be storing the vertices in this struct, or should I just store the vbo id?
    core::arr<core::vec2f> m_vertices;
    u32 m_vboId;
    u32 m_vaoId;
};
