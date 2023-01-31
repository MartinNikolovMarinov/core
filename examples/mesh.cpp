#include "mesh.h"


u32 Mesh2D::Usage::usage_Gl() const {
    u32 ret = -1;
    if (a == Access::STATIC) {
        if (t == AccessType::DRAW) ret = GL_STATIC_DRAW;
        if (t == AccessType::READ) ret = GL_STATIC_READ;
        if (t == AccessType::COPY) ret = GL_STATIC_COPY;
    }
    if (a == Access::DYNAMIC) {
        if (t == AccessType::DRAW) ret = GL_DYNAMIC_DRAW;
        if (t == AccessType::READ) ret = GL_DYNAMIC_READ;
        if (t == AccessType::COPY) ret = GL_DYNAMIC_COPY;
    }
    if (a == Access::STREAM) {
        if (t == AccessType::DRAW) ret = GL_STREAM_DRAW;
        if (t == AccessType::READ) ret = GL_STREAM_READ;
        if (t == AccessType::COPY) ret = GL_STREAM_COPY;
    }
    return ret;
}

Mesh2D Mesh2D::create(const Mesh2D::VertexLayout& vl, core::arr<core::vec2f>&& vertices) {
    Mesh2D m;
    m.m_vertices = core::move(vertices);

    u32 vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    auto bufferDataSize = m.m_vertices.len() * sizeof(core::vec2f);
    glBufferData(GL_ARRAY_BUFFER, bufferDataSize, m.m_vertices.data(), vl.usage.usage_Gl());
    return m;
}

void Mesh2D::destroy() const {
    glDeleteBuffers(1, &m_vboId);
}

i32 Mesh2D::vertex_count() const {
    return m_vertices.len();
}

void Mesh2D::bind() const {
    static u32 lastBoundVboId = 0; // cache the last bound vbo.
    if (lastBoundVboId == m_vboId) return;
    glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
    lastBoundVboId = m_vboId;
}
