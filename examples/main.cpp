// IMPORTANT: The init_core.h should be included before anything else has the chance to include core.h.
#include <init_core.h>

#include <fmt/format.h>

#include "ex/common.h"
#include "ex/experiment_01_triangle_and_quad.h"

// TODO: Drawing line segments is quite the problem to solve in one pass. I first need to:
//        - Go through all tutorials on https://learnopengl.com/ and make sure I understand everything.
//        - Leave some good working examples with as little abstraciton as possible, without repeating code too much.
//        - Examples need to be easy to switch between and test new things.
//        - Read very carefully about instanced rendering in OpenGL from https://learnopengl.com/Advanced-OpenGL/Instancing
//        - Then trying to slowly apply this to the examples here - https://wwwtyro.net/2019/11/18/instanced-lines.html
//        - Maybe explore more techniques for rendering many line segments.
//          Although this should be good enough for most applications. Especially for my needs.

struct Example {
    i32 (*init)(common::AppState& s);
    void (*destroy)();
    void (*preMainLoop)(common::AppState& s);
    void (*mainLoop)(common::AppState& s);
};

inline i32 runExample(Example&& ex) {
    {
        defer {
            ex.destroy();
            common::destroy();
        };
        common::InitProps iprops;
        iprops.title = "Experiment 01 - Triangle";
        iprops.width = 800;
        iprops.height = 600;
        iprops.clearColor = {0.2f, 0.3f, 0.3f, 1.0f};
        iprops.initStateCb = ex.init;
        iprops.preMainLoopCb = ex.preMainLoop;
        iprops.mainLoopCb = ex.mainLoop;
        iprops.debugWireFrameMode = false;
        if (i32 err = common::init(core::move(iprops)); err != common::AppExitCodes::APP_EXIT_SUCCESS) {
            return err;
        }
        if (i32 err = common::run(); err != common::AppExitCodes::APP_EXIT_SUCCESS) {
            return err;
        }
    }

    ptr_size leaked = core::used_mem<CORE_DEFAULT_ALLOCATOR()>();
    if (leaked > 0) {
        fmt::print(stderr, "Leaked {} bytes of memory\n", leaked);
        return common::AppExitCodes::APP_EXIT_MEMORY_LEAK;
    } else {
        fmt::print("No memory leaks detected\n");
        return common::AppExitCodes::APP_EXIT_SUCCESS;
    }
}

i32 example_app() {
    return 0;
}

i32 example_01() {
    Example ex;
    ex.init = triangle_and_quad_ex_01::init;
    ex.destroy = triangle_and_quad_ex_01::destroy;
    ex.preMainLoop = triangle_and_quad_ex_01::preMainLoop;
    ex.mainLoop = triangle_and_quad_ex_01::mainLoop;
    return runExample(core::move(ex));
}

i32 main(i32, char const**) {
    return example_01();
}
