#pragma once

#include "init_core.h"
#include "shader_prog.h"

struct Box {
    core::mat4f transform = core::mat4x4f::identity();
    core::vec4f color;

    void scale(const core::vec3f& scale);
    void rotate(const core::vec3f& axis, const core::radians& angle);
    void translate(const core::vec3f& translation);
    void setColor(const core::vec4f& color);
};

void immediate_cubeRendererInit(u32 inPosAttribLocation);
void immediate_cubeRendererDestroy();
void immediate_render(ShaderProg& prog, const Box& cube);
