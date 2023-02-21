#pragma once

#include <init_core.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fmt/format.h>

#include <string>

namespace common {

enum AppExitCodes : i32 {
    APP_EXIT_SUCCESS = 0,
    APP_EXIT_FAILED_TO_INIT = -1,
    APP_EXIT_MEMORY_LEAK = -2,
};

struct Window {
    GLFWwindow* glfwWindow;
    i32 width;
    i32 height;
    const char* title;
};

struct CommonState;

struct GraphicsLibError {
    i32 code;
    std::string msg;
};

using InitStateCb = core::expected<GraphicsLibError> (*)(CommonState& s);
using PreMainLoopCb = core::expected<GraphicsLibError> (*)(CommonState& s);
using MainLoopCb = void (*)(CommonState& s);

struct CommonState {
    u64 firstFrameTimestamp_ms = 0;        // timestamp of the first frame in milliseconds.
    u64 frameCount = 0;                    // number of frames since program started.
    f64 fps = 0;                           // frames per second.
    PreMainLoopCb preMainLoopCb = nullptr; // user defined callback that will be called once before the main loop starts.
    MainLoopCb mainLoopCb = nullptr;       // user defined callback that will be called on every frame.
    bool waitForEvents = false;            // the main loop will wait for events instead of polling.

    Window mainWindow = {};      // the main window of the app.
    core::vec4f clearColor = {}; // the default background color.

    bool debugWireFrameMode = false; // render in wireframe mode.
};

struct InitProps {
    const char* title = nullptr;
    i32 width = 0;
    i32 height = 0;
    core::vec4f clearColor = {};
    InitStateCb initStateCb = nullptr;
    PreMainLoopCb preMainLoopCb = nullptr;
    MainLoopCb mainLoopCb = nullptr;
    bool debugWireFrameMode = false;
    bool waitForEvents = false;
};

CommonState& state();
core::expected<GraphicsLibError> init(InitProps&& props);
i32 run();
void destroy();

} // namespace common
