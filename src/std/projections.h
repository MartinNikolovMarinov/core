#pragma once

#include <types.h>
#include <std/mat.h>

#include <cmath>

namespace core {

using namespace coretypes;

// IMPORTANT: Device coordinates are assumed to be from -1 to 1

constexpr mat4x4f ortho(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far) {
    auto ret = mat4x4f::identity();
    ret[0][0] = 2.0f / (right - left);
    ret[1][1] = 2.0f / (top - bottom);
    ret[2][2] = -2.0f / (far - near);
    ret[3][0] = -(right + left) / (right - left);
    ret[3][1] = -(top + bottom) / (top - bottom);
    ret[3][2] = -(far + near) / (far - near);
    return ret;
}

constexpr mat4x4f ortho(f32 left, f32 right, f32 bottom, f32 top) {
    return ortho(left, right, bottom, top, -1.0f, 1.0f);
}

constexpr mat4x4f orthoLH(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far) {
    auto ret = mat4x4f::identity();
    ret[0][0] = 2.0f / (right - left);
    ret[1][1] = 2.0f / (top - bottom);
    ret[2][2] = 2.0f / (far - near);
    ret[3][0] = -(right + left) / (right - left);
    ret[3][1] = -(top + bottom) / (top - bottom);
    ret[3][2] = -(far + near) / (far - near);
    return ret;
}

constexpr mat4x4f orthoLH(f32 left, f32 right, f32 bottom, f32 top) {
    return orthoLH(left, right, bottom, top, -1.0f, 1.0f);
}

constexpr mat4x4f perspective(f32 fovy, f32 aspect, f32 near, f32 far) {
    auto ret = mat4x4<f32>(0);
    f32 tanHalfFov = std::tan(fovy / 2.0f);
    ret[0][0] = 1.0f / (aspect * tanHalfFov);
    ret[1][1] = 1.0f / (tanHalfFov);
    ret[2][2] = -(far + near) / (far - near);
    ret[2][3] = -1.0f;
    ret[3][2] = -(2.0f * far * near) / (far - near);
    return ret;
}

constexpr mat4x4f perspectiveLH(f32 fovy, f32 aspect, f32 near, f32 far) {
    auto ret = mat4x4<f32>(0);
    f32 tanHalfFov = std::tan(fovy / 2.0f);
    ret[0][0] = 1.0f / (aspect * tanHalfFov);
    ret[1][1] = 1.0f / (tanHalfFov);
    ret[2][2] = (far + near) / (far - near);
    ret[2][3] = 1.0f;
    ret[3][2] = (2.0f * far * near) / (far - near);
    return ret;
}

} // namespace core
