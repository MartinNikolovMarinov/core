#pragma once

#include <core_API.h>
#include <core_types.h>
#include <math/core_math.h>
#include <math/core_matrix.h>

namespace core {

using namespace coretypes;

// IMPORTANT: Device coordinates are assumed to be from -1 to 1

CORE_API_EXPORT mat4x4f ortho(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);
CORE_API_EXPORT mat4x4f ortho(f32 left, f32 right, f32 bottom, f32 top);
CORE_API_EXPORT mat4x4f orthoLH(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);
CORE_API_EXPORT mat4x4f orthoLH(f32 left, f32 right, f32 bottom, f32 top);
CORE_API_EXPORT mat4x4f perspective(f32 fovy, f32 aspect, f32 near, f32 far);
CORE_API_EXPORT mat4x4f perspectiveLH(f32 fovy, f32 aspect, f32 near, f32 far);

} // namespace core
