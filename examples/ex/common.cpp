#include "common.h"

namespace common {

CommonState& state() {
    static CommonState g_state = {};
    return g_state;
}

namespace {

GraphicsLibError createGLFWErr() {
    GraphicsLibError err;
    const char* errDesc = nullptr;
    err.code = glfwGetError(&errDesc);
    err.msg = fmt::format("Failed to initialize GLFW reason: {}", errDesc ? errDesc : "Unknown");
    return err;
}

core::expected<GraphicsLibError> initGlfw() {
    fmt::print("GLFW version: {}\n", glfwGetVersionString());

    if (glfwInit() != GLFW_TRUE) {
        return core::unexpected(createGLFWErr());
    }

    // Setup GLFW window hints:
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#ifdef OS_MAC
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_SAMPLES, 4);

    return {};
}

core::expected<GraphicsLibError> initGlew() {
    if (auto errCode = glewInit(); errCode != GLEW_OK) {
        GraphicsLibError err;
        const char* errDesc = reinterpret_cast<const char*>(glewGetErrorString(errCode));
        err.code = errCode;
        err.msg = fmt::format("Failed to initialize GLEW reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }
    return {};
}

void initOpenGL() {
    CommonState& g_s = state();
    glViewport(0, 0, g_s.mainWindow.width, g_s.mainWindow.height);

    auto& cc = g_s.clearColor;
    glClearColor(cc.r(), cc.g(), cc.b(), cc.a());

    if (g_s.debugWireFrameMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // TODO2: Might want to disable this if I want to implement anti-alising myself.
    //       Or if I deciede to render to a backbuffer/texture. Leave it for the examples for now.
    // Enable building anti-aliasing:
    glEnable(GL_MULTISAMPLE);
}

}

core::expected<GraphicsLibError> init(InitProps&& props) {
    Assert(props.mainLoopCb != nullptr, "Provided nullptr for Main Loop.");

    ValueOrReturn(initGlfw());

    CommonState& g_s = state();
    g_s.mainLoopCb = props.mainLoopCb;
    g_s.preMainLoopCb = props.preMainLoopCb;
    g_s.debugWireFrameMode = props.debugWireFrameMode;
    g_s.clearColor = props.clearColor;
    g_s.waitForEvents = props.waitForEvents;
    g_s.frameCount = 0;

    // Create the glfw main window:
    g_s.mainWindow = {};
    g_s.mainWindow.width = props.width;
    g_s.mainWindow.height = props.height;
    g_s.mainWindow.title = props.title;
    g_s.mainWindow.glfwWindow = glfwCreateWindow(g_s.mainWindow.width, g_s.mainWindow.height, g_s.mainWindow.title, 0, 0);
    if (!g_s.mainWindow.glfwWindow) {
        return core::unexpected(createGLFWErr());
    }

    glfwMakeContextCurrent(g_s.mainWindow.glfwWindow);
    // This sets the modifier bit for caps lock and num lock when a key press event is received.
    // These modifiers need to be locked because there is no need to hold them:
    glfwSetInputMode(g_s.mainWindow.glfwWindow, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
    // Enable vsync:
    glfwSwapInterval(1);

    // Init glew after we have a window:
    ValueOrReturn(initGlew());

    // Initialize OpenGL:
    initOpenGL();

    // Call the custom user provided callback:
    if (props.initStateCb) {
        ValueOrReturn(props.initStateCb(g_s))
    }

    return {};
}

void destroy() {
    CommonState& g_s = state();
    if (g_s.mainWindow.glfwWindow) glfwDestroyWindow(g_s.mainWindow.glfwWindow);
    glfwTerminate();
}

i32 run() {
    CommonState& g_s = state();

    if (g_s.preMainLoopCb) {
        auto ret = g_s.preMainLoopCb(g_s);
        if (ret.has_err()) {
            fmt::print("User defined Pre-Main Loop Callback failed; reason: {}\n", ret.err().msg);
            return AppExitCodes::APP_EXIT_FAILED_TO_INIT;
        }
    }

    constexpr u64 minFramesPerMeasure = 10;
    constexpr f64 minTimeForMeasure_seconds = 0.25;
    u64 measureStartTime_ms = ValueOrDie(core::os_unix_time_stamp_in_ms());
    u64 framesSinceMeasure = 0;

    while (!glfwWindowShouldClose(g_s.mainWindow.glfwWindow)) {
        u64 elapsedTime_ms = ValueOrDie(core::os_unix_time_stamp_in_ms());
        g_s.frameCount++;
        framesSinceMeasure++;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (g_s.waitForEvents) glfwWaitEvents();
        else glfwPollEvents();

        g_s.mainLoopCb(g_s);

        f64 diff_seconds = (elapsedTime_ms - measureStartTime_ms) / 1000.0;
        if (diff_seconds > minTimeForMeasure_seconds && framesSinceMeasure > minFramesPerMeasure) {
            g_s.fps = f64(framesSinceMeasure) / diff_seconds;
            measureStartTime_ms = elapsedTime_ms;
            framesSinceMeasure = 0;
        }
    }

    return AppExitCodes::APP_EXIT_SUCCESS;
}

} // namespace common
