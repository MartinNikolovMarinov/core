#pragma once

#include "common.h"

namespace triangle_and_quad_ex_01 {

using AppExitCodes = common::AppExitCodes;
using CommonState = common::CommonState;
using GraphicsLibError = common::GraphicsLibError;

core::expected<GraphicsLibError> init(CommonState &s);
void destroy();
core::expected<GraphicsLibError> preMainLoop(CommonState& s);
void mainLoop(CommonState& s);

} // namespace triangle_and_quad_ex_01
