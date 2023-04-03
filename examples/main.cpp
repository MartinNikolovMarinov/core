// IMPORTANT: The init_core.h should be included before anything else has the chance to include core.h.
#include <init_core.h>

#include <fmt/format.h>

#include "ex/common.h"
#include "ex/experiment_01_triangle_and_quad.h"
#include "ex/experiment_02_textures.h"
#include "ex/experiment_03_transformations.h"
#include "ex/experiment_04_rotating_cube.h"
#include "ex/experiment_app.h"
#include "ex/raytracing.h"
#include "ex/ray_in_voxel_space.h"

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
    core::vec4f clearColor = {0.2f, 0.3f, 0.3f, 1.0f};
};

inline i32 runExample(Example&& ex) {
    auto checkForLeaks = []() {
        ptr_size leaked = core::used_mem<CORE_DEFAULT_ALLOCATOR()>();
        if (leaked > 0) {
            fmt::print(stderr, "Leaked {} bytes of memory\n", leaked);
            core::os_exit(common::AppExitCodes::APP_EXIT_MEMORY_LEAK);
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
    iprops.clearColor = ex.clearColor;
    iprops.initStateCb = ex.init;
    iprops.preMainLoopCb = ex.preMainLoop;
    iprops.mainLoopCb = ex.mainLoop;
    iprops.waitForEvents = ex.waitForEvents;
    iprops.debugWireFrameMode = true;
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

i32 example_raytracing() {
    Example ex;
    ex.init = raytracing::init;
    ex.destroy = raytracing::destroy;
    ex.preMainLoop = raytracing::preMainLoop;
    ex.mainLoop = raytracing::mainLoop;
    ex.waitForEvents = false;
    return runExample(core::move(ex));
}

i32 example_ray_in_voxel_space() {
    Example ex;
    ex.init = ray_in_voxel_space::init;
    ex.destroy = ray_in_voxel_space::destroy;
    ex.preMainLoop = ray_in_voxel_space::preMainLoop;
    ex.mainLoop = ray_in_voxel_space::mainLoop;
    ex.waitForEvents = false;
    ex.clearColor = core::WHITE;
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

i32 example_02() {
    Example ex;
    ex.init = textures_ex_02::init;
    ex.destroy = textures_ex_02::destroy;
    ex.preMainLoop = textures_ex_02::preMainLoop;
    ex.mainLoop = textures_ex_02::mainLoop;
    ex.waitForEvents = false;
    return runExample(core::move(ex));
}

i32 example_03() {
    Example ex;
    ex.init = transformation_ex_03::init;
    ex.destroy = transformation_ex_03::destroy;
    ex.preMainLoop = transformation_ex_03::preMainLoop;
    ex.mainLoop = transformation_ex_03::mainLoop;
    ex.waitForEvents = false;
    return runExample(core::move(ex));
}

// First real 3D example.
i32 example_04() {
    Example ex;
    ex.init = rotating_cube_ex_04::init;
    ex.destroy = rotating_cube_ex_04::destroy;
    ex.preMainLoop = rotating_cube_ex_04::preMainLoop;
    ex.mainLoop = rotating_cube_ex_04::mainLoop;
    ex.waitForEvents = false;
    return runExample(core::move(ex));
}

i32 main(i32, char const**) {
    initCore();
    return ray_in_voxel_space();
}
