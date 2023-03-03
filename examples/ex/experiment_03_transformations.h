#pragma once

#include "common.h"

namespace transformation_ex_03 {

using AppExitCodes = common::AppExitCodes;
using CommonState = common::CommonState;
using GraphicsLibError = common::GraphicsLibError;

core::expected<GraphicsLibError> init(CommonState& s);
void destroy();
core::expected<GraphicsLibError> preMainLoop(CommonState& s);
void mainLoop(CommonState& s);

} // namespace transformation_ex_03
