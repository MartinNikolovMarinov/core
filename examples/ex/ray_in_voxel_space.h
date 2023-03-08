#pragma once

#include "common.h"

namespace ray_in_voxel_space {

using AppExitCodes = common::AppExitCodes;
using CommonState = common::CommonState;
using GraphicsLibError = common::GraphicsLibError;

core::expected<GraphicsLibError> init(CommonState& s);
void destroy();
core::expected<GraphicsLibError> preMainLoop(CommonState& s);
void mainLoop(CommonState& s);

} // namespace ray_in_voxel_space

