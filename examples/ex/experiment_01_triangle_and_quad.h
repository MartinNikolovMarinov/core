#pragma once

#include "common.h"

namespace triangle_and_quad_ex_01 {

using AppExitCodes = common::AppExitCodes;
using AppState = common::AppState;

i32 init(AppState &s);
void destroy();
void preMainLoop(AppState& s);
void mainLoop(AppState& s);

} // namespace triangle_and_quad_ex_01
