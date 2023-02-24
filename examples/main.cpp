// IMPORTANT: The init_core.h should be included before anything else has the chance to include core.h.
#include <init_core.h>

#include <fmt/format.h>

#include "ex/common.h"
#include "ex/experiment_01_triangle_and_quad.h"
#include "ex/experiment_app.h"

// TODO: Drawing line segments is quite the problem to solve in one pass. I first need to:
//        - Go through all tutorials on https://learnopengl.com/ and make sure I understand everything.
//        - Leave some good working examples with as little abstraciton as possible, without repeating code too much.
//        - Examples need to be easy to switch between and test new things.
//        - Read very carefully about instanced rendering in OpenGL from https://learnopengl.com/Advanced-OpenGL/Instancing
//        - Then trying to slowly apply this to the examples here - https://wwwtyro.net/2019/11/18/instanced-lines.html
//        - Maybe explore more techniques for rendering many line segments.
//          Although this should be good enough for most applications. Especially for my needs.

struct Example {
    common::InitStateCb init = nullptr;
    common::PreMainLoopCb preMainLoop = nullptr;
    common::MainLoopCb mainLoop = nullptr;
    void (*destroy)() = nullptr;
    bool waitForEvents = false;
};

inline i32 runExample(Example&& ex) {
    auto checkForLeaks = []() {
        ptr_size leaked = core::used_mem<CORE_DEFAULT_ALLOCATOR()>();
        if (leaked > 0) {
            fmt::print(stderr, "Leaked {} bytes of memory\n", leaked);
            std::exit(common::AppExitCodes::APP_EXIT_MEMORY_LEAK);
            return;
        }
        fmt::print("No memory leaks detected\n");
    };

    defer {
        ex.destroy();
        common::destroy();
        checkForLeaks();
    };

    common::InitProps iprops;
    iprops.title = "Experiment 01 - Triangle";
    iprops.width = 800;
    iprops.height = 600;
    iprops.clearColor = {0.2f, 0.3f, 0.3f, 1.0f};
    iprops.initStateCb = ex.init;
    iprops.preMainLoopCb = ex.preMainLoop;
    iprops.mainLoopCb = ex.mainLoop;
    iprops.waitForEvents = ex.waitForEvents;
    iprops.debugWireFrameMode = false;
    if (auto err = common::init(core::move(iprops)); err.has_err()) {
        fmt::print(stderr, "Failed to initialize the application: {}\n", err.err().msg.c_str());
        return common::AppExitCodes::APP_EXIT_FAILED_TO_INIT;
    }

    if (i32 err = common::run(); err != common::AppExitCodes::APP_EXIT_SUCCESS) {
        return err;
    }

    return common::AppExitCodes::APP_EXIT_SUCCESS;
}

i32 example_app() {
    Example ex;
    ex.init = app::init;
    ex.destroy = app::destroy;
    ex.preMainLoop = app::preMainLoop;
    ex.mainLoop = app::mainLoop;
    ex.waitForEvents = true;
    return runExample(core::move(ex));
}

i32 example_01() {
    Example ex;
    ex.init = triangle_and_quad_ex_01::init;
    ex.destroy = triangle_and_quad_ex_01::destroy;
    ex.preMainLoop = triangle_and_quad_ex_01::preMainLoop;
    ex.mainLoop = triangle_and_quad_ex_01::mainLoop;
    ex.waitForEvents = false;
    return runExample(core::move(ex));
}

i32 main(i32, char const**) {
    return example_01();
}
