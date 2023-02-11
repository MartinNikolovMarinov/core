#include "shape.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

u32 Shape2D::Usage::usageGL() const {
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

u32 Shape2D::RenderMode::modeGL() const {
    u32 ret = -1;
    switch (m) {
        case Mode::POINTS:         ret = GL_POINTS;         break;
        case Mode::LINES:          ret = GL_LINES;          break;
        case Mode::LINE_STRIP:     ret = GL_LINE_STRIP;     break;
        case Mode::LINE_LOOP:      ret = GL_LINE_LOOP;      break;
        case Mode::TRIANGLES:      ret = GL_TRIANGLES;      break;
        case Mode::TRIANGLE_STRIP: ret = GL_TRIANGLE_STRIP; break;
        case Mode::TRIANGLE_FAN:   ret = GL_TRIANGLE_FAN;   break;
    }
    return ret;
}

Shape2D Shape2D::create(const Shape2D::VertexLayout& vl,
                        const core::vec4u& colorRGB,
                        f32 zIndex,
                        core::arr<core::vec2f>&& vertices) {
    Shape2D m;
    m.m_vertices = core::move(vertices);
    m.m_vboId = -1;
    m.m_vaoId = -1;
    m.m_zIndex = zIndex;
    m.m_renderMode = vl.renderMode;
    m.m_color = {
        f32(colorRGB.r()) / 255.0f, f32(colorRGB.g()) / 255.0f,
        f32(colorRGB.b()) / 255.0f, f32(colorRGB.z()) / 255.0f,
    };

    // Create VBO
    glGenBuffers(1, &m.m_vboId);
    glBindBuffer(GL_ARRAY_BUFFER, m.m_vboId);
    ptr_size bufferDataSize = m.m_vertices.len() * sizeof(core::vec2f);
    glBufferData(GL_ARRAY_BUFFER, bufferDataSize, m.m_vertices.data(), vl.usage.usageGL());

    // Create VAO
    glGenVertexArrays(1, &m.m_vaoId);
    glBindVertexArray(m.m_vaoId);

    // Link vertex attributes
    constexpr ptr_size stride = sizeof(core::vec2f);
    constexpr ptr_size dimentions = core::vec2f::dimentions();
    glVertexAttribPointer(vl.posAttribId, dimentions, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(vl.posAttribId);

    return m;
}

Shape2D Shape2D::createRect2D(const Shape2D::VertexLayout& vl,
                              core::vec2f pos, f32 width, f32 height,
                              const core::vec4u& colorRGB,
                              f32 zIdx) {
    Assert(width > 0.0f && height > 0.0f);

    // Center the rect around the position:
    pos -= core::v(width / 2.0f, height / 2.0f);

    core::arr<core::vec2f> vertices(0, 6);
    vertices.append(core::v(pos.x() + width, pos.y()))
            .append(core::v(pos.x(), pos.y()))
            .append(core::v(pos.x(), pos.y() + height))
            .append(core::v(pos.x() + width, pos.y()))
            .append(core::v(pos.x() + width, pos.y() + height))
            .append(core::v(pos.x(), pos.y() + height));

    Shape2D rectShape = Shape2D::create(vl, colorRGB, zIdx, core::move(vertices));
    return rectShape;
}

Shape2D Shape2D::createTriangle2D(const Shape2D::VertexLayout &vl,
                                  core::vec2f a, core::vec2f b, core::vec2f c,
                                  const core::vec4u &colorRGB,
                                  f32 zIdx) {
    core::arr<core::vec2f> vertices(0, 3);
    vertices.append(a).append(b).append(c);
    Shape2D triangleShape = Shape2D::create(vl, colorRGB, zIdx, core::move(vertices));
    return triangleShape;
}

Shape2D Shape2D::createCircle2D(const Shape2D::VertexLayout& vl,
                                core::vec2f center, f32 radius, u32 steps,
                                const core::vec4u& colorRGB, f32 zIdx) {
    Assert(radius > 0.0f);

    core::arr<core::vec2f> vertices(0, steps);
    f32 angleStep = core::deg_to_rad(360.0f / f32(steps));
    for (u32 i = 0; i < steps - 1; ++i) {
        f32 angle = angleStep * i;
        core::vec2f pos = core::v(center.x() + radius * std::cos(angle),
                                  center.y() + radius * std::sin(angle));
        vertices.append(pos);
    }
    vertices.append(vertices.first());

    Shape2D circleShape = Shape2D::create(vl, colorRGB, zIdx, core::move(vertices));
    return circleShape;
}

u32 Shape2D::vboID() const { return m_vboId; }
u32 Shape2D::vaoID() const { return m_vaoId; }
f32 Shape2D::zIdx() const { return m_zIndex; }
const core::arr<core::vec2f>& Shape2D::vertices() const { return m_vertices; }
i32 Shape2D::vertexCount() const { return m_vertices.len(); }
core::vec4u Shape2D::colorRGB() const { return core::v_conv<core::vec4u>(m_color * 255.0f); }
const core::vec4f& Shape2D::color() const { return m_color; }
const Shape2D::RenderMode& Shape2D::renderMode() const { return m_renderMode; }

void Shape2D::destroy() const {
    glDeleteBuffers(1, &m_vboId);
    glDeleteVertexArrays(1, &m_vaoId);
}
