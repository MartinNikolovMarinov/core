#pragma once

#include <init_core.h>

namespace common {

enum AppExitCodes : i32 {
    APP_EXIT_SUCCESS = 0,
    APP_EXIT_FAILED_TO_INIT = -1,
    APP_EXIT_MEMORY_LEAK = -2,
};

struct AppState;
struct Window;

using InitStateCb = i32 (*)(AppState& s);
using PreMainLoopCb = void (*)(AppState& s);
using MainLoopCb = void (*)(AppState& s);

struct AppState {
    u64 firstFrameTimestamp_ms;  // timestamp of the first frame in milliseconds.
    u64 frameCount;              // number of frames since the app started.
    PreMainLoopCb preMainLoopCb; // user defined callback that will be called once before the main loop starts.
    MainLoopCb mainLoopCb;       // user defined callback that will be called on every frame.

    Window* mainWindow;     // the main window of the app. TODO: replace with my unique ptr implementation when ready.
    core::vec4f clearColor; // the default background color.

    bool debugWireFrameMode = false; // render in wireframe mode.
};

struct InitProps {
    const char* title;
    i32 width;
    i32 height;
    core::vec4f clearColor;
    InitStateCb initStateCb;
    PreMainLoopCb preMainLoopCb;
    MainLoopCb mainLoopCb; // required.
    bool debugWireFrameMode = false;
};

void* getNativeWindowHandle(Window* w);

AppState& state();
i32 init(InitProps&& props);
i32 run();
void destroy();

} // namespace common
