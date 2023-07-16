// IMPORTANT: The init_core.h should be included before anything else has the chance to include core.h.
#include <init_core.h>

#include <fmt/format.h>

#include "ex/common.h"
#include "ex/experiment_01_triangle_and_quad.h"
#include "ex/experiment_02_textures.h"
#include "ex/experiment_03_transformations.h"
#include "ex/experiment_04_rotating_cube.h"
#include "ex/experiment_05_camera.h"
#include "ex/raytracing.h"
#include "ex/ray_in_voxel_space.h"
#include "ex/keyboard_and_mouse.h"

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
    core::vec4f clearColor = core::v(0.2f, 0.3f, 0.3f, 1.0f);
    i32 windowWidth;
    i32 windowHeight;
    const char* title;
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
    iprops.title = ex.title;
    iprops.width = ex.windowWidth;
    iprops.height = ex.windowHeight;
    iprops.clearColor = ex.clearColor;
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

i32 example_raytracing() {
    Example ex;
    ex.init = raytracing::init;
    ex.destroy = raytracing::destroy;
    ex.preMainLoop = raytracing::preMainLoop;
    ex.mainLoop = raytracing::mainLoop;
    ex.waitForEvents = false;
    // ex.windowWidth = 1920;
    // ex.windowHeight = 1080;
    ex.windowWidth = 400;
    ex.windowHeight = 300;
    ex.title = "Raytracing";
    return runExample(core::move(ex));
}

i32 example_ray_in_voxel_space() {
    Example ex;
    ex.init = ray_in_voxel_space::init;
    ex.destroy = ray_in_voxel_space::destroy;
    ex.preMainLoop = ray_in_voxel_space::preMainLoop;
    ex.mainLoop = ray_in_voxel_space::mainLoop;
    ex.waitForEvents = false;
    ex.windowWidth = 800;
    ex.windowHeight = 600;
    ex.clearColor = core::WHITE;
    ex.title = "Ray in Voxel Space";
    return runExample(core::move(ex));
}

i32 example_keyboard_and_mouse() {
    Example ex;
    ex.init = keyboard_and_mouse::init;
    ex.destroy = keyboard_and_mouse::destroy;
    ex.preMainLoop = keyboard_and_mouse::preMainLoop;
    ex.mainLoop = keyboard_and_mouse::mainLoop;
    ex.waitForEvents = true;
    ex.windowWidth = 800;
    ex.windowHeight = 600;
    ex.title = "Keyboard and Mouse";
    return runExample(core::move(ex));
}

i32 example_01() {
    Example ex;
    ex.init = triangle_and_quad_ex_01::init;
    ex.destroy = triangle_and_quad_ex_01::destroy;
    ex.preMainLoop = triangle_and_quad_ex_01::preMainLoop;
    ex.mainLoop = triangle_and_quad_ex_01::mainLoop;
    ex.waitForEvents = false;
    ex.windowWidth = 800;
    ex.windowHeight = 600;
    ex.title = "Triangle and Quad Example 01";
    return runExample(core::move(ex));
}

i32 example_02() {
    Example ex;
    ex.init = textures_ex_02::init;
    ex.destroy = textures_ex_02::destroy;
    ex.preMainLoop = textures_ex_02::preMainLoop;
    ex.mainLoop = textures_ex_02::mainLoop;
    ex.waitForEvents = false;
    ex.windowWidth = 800;
    ex.windowHeight = 600;
    ex.title = "Textures Example 02";
    return runExample(core::move(ex));
}

i32 example_03() {
    Example ex;
    ex.init = transformation_ex_03::init;
    ex.destroy = transformation_ex_03::destroy;
    ex.preMainLoop = transformation_ex_03::preMainLoop;
    ex.mainLoop = transformation_ex_03::mainLoop;
    ex.waitForEvents = false;
    ex.windowWidth = 800;
    ex.windowHeight = 600;
    ex.title = "Transformation Example 03";
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
    ex.windowWidth = 800;
    ex.windowHeight = 600;
    ex.title = "Rotating Cube Example 04";
    return runExample(core::move(ex));
}

i32 example_05() {
    Example ex;
    ex.init = camera_ex_05::init;
    ex.destroy = camera_ex_05::destroy;
    ex.preMainLoop = camera_ex_05::preMainLoop;
    ex.mainLoop = camera_ex_05::mainLoop;
    ex.waitForEvents = false;
    ex.windowWidth = 800;
    ex.windowHeight = 600;
    ex.title = "Camera Example 05";
    return runExample(core::move(ex));
}

struct FlagParser {
    static constexpr i32 MAX_ARG_LEN = 5000;

    FlagParser(i32 maxArgLen = MAX_ARG_LEN) : m_maxArgLen(maxArgLen) {}

    // TODO: optional type !
    enum FlagType {
        Bool,
        Int32,
        Int64,
        Uint32,
        Uint64,
        Float,
        CPtr,
    };

    struct Flag {
        void* arg;
        const char* name;
        i32 nameLen;
        FlagType type;
        bool flagSet;
    };

    enum ParseErr {
        Success = 0,
        ArgLenTooLong = 1,
        UnknownFlag = 2,
    };

    core::expected<ParseErr> parse(i32 argc, const char** argv) {
        i32 state = 0;
        Flag* curFlag = nullptr;

        for (i32 i = 0; i < argc; ++i) {
            const char* curVal = argv[i];
            if (curVal == nullptr) continue;
            curVal = core::cptr_skip_space(curVal);
            if (curVal[0] == '-' && state == 0) {
                curVal++;
                state = 1;
                if (curVal[0] == '-') curVal++; // allow long -- flags
            }
            i32 valLen = 0;
            while (curVal[valLen]) {
                valLen++;
                if (valLen > m_maxArgLen) {
                    return core::unexpected(ParseErr::ArgLenTooLong);
                }
            }

           i32 reducedValLen = valLen;
            while (core::is_white_space(curVal[reducedValLen - 1])) reducedValLen--;

            if (state == 1) {
                i32 fidx = core::find(m_flags, [&](const Flag& f, i32) -> bool {
                    i32 reducedNameLen = f.nameLen;
                    while (core::is_white_space(f.name[reducedNameLen - 1])) reducedNameLen--;
                    bool areEqual = core::cptr_cmp(f.name, reducedNameLen, curVal, reducedValLen) == 0;
                    return areEqual;
                });
                if (fidx == -1) {
                    return core::unexpected(ParseErr::UnknownFlag);
                }

                curFlag = &m_flags[fidx];
                state = 2;
            }
            else if (state == 2) {
                Assert(curFlag != nullptr, "Implementation bug.");
                switch (curFlag->type) {
                    case FlagType::Bool:
                    {
                        bool v = curVal[0] == '1';
                        if (!v) {
                            if (reducedValLen == 1) {
                                v = curVal[0] == 't' || curVal[0] == 'T';
                            }
                            else if (reducedValLen == 4) {
                                v = core::cptr_cmp(curVal, 4, "true", 4) == 0 ||
                                    core::cptr_cmp(curVal, 4, "True", 4) == 0 ||
                                    core::cptr_cmp(curVal, 4, "TRUE", 4) == 0;
                            }
                        }
                        *(bool*)curFlag->arg = v;
                        break;
                    }
                    case FlagType::Int32:
                        *(i32*)curFlag->arg = core::cptr_to_int<i32>(curVal);
                        break;
                    case FlagType::Int64:
                        *(i64*)curFlag->arg = core::cptr_to_int<i64>(curVal);
                        break;
                    case FlagType::Uint32:
                        *(u32*)curFlag->arg = core::cptr_to_int<u32>(curVal);
                        break;
                    case FlagType::Uint64:
                        *(u64*)curFlag->arg = core::cptr_to_int<u64>(curVal);
                        break;
                    case FlagType::Float:
                        // TODO: implement this
                        break;
                    case FlagType::CPtr:
                        *(const char**)curFlag->arg = curVal;
                        curFlag->flagSet = true;
                        break;
                    default:
                        return core::unexpected(ParseErr::UnknownFlag);
                }

                state = 0;
            }
        }

        return {};
    }

    void flag(char** out, const char* flagName)     { setFlag(out, flagName, FlagType::CPtr); }
    void flagInt32(i32* out, const char* flagName)  { setFlag(out, flagName, FlagType::Int32); }
    void flagInt64(i64* out, const char* flagName)  { setFlag(out, flagName, FlagType::Int64); }
    void flagUint32(u32* out, const char* flagName) { setFlag(out, flagName, FlagType::Uint32); }
    void flagUint64(u64* out, const char* flagName) { setFlag(out, flagName, FlagType::Uint64); }
    void flagBool(bool* out, const char* flagName)  { setFlag(out, flagName, FlagType::Bool); }

private:
    core::arr<Flag> m_flags;
    i32 m_maxArgLen;

    template <typename T>
    void setFlag(T* out, const char* flagName, FlagType type) {
        Flag f;
        f.arg = (void*)out;
        f.name = core::cptr_skip_space(flagName);
        f.nameLen = core::cptr_len(f.name);
        f.type = type;
        f.flagSet = false;
        m_flags.append(f);
    }
};

// TODO: use valgrind to check for memory access violations.

i32 main(i32 argc, const char** argv) {
    initCore();

    FlagParser parser;

    i32 a = 0;
    i64 b = 0;
    u32 c = 0;
    u64 d = 0;
    parser.flagInt32(&a, " int32");
    parser.flagInt64(&b, "int64 ");
    parser.flagUint32(&c, "uint32\t");
    parser.flagUint64(&d, "\nuint64");

    char* strArg = nullptr;
    parser.flag(&strArg, "string");

    bool bool_1 = false;
    bool bool_2 = false;
    bool bool_3 = false;
    bool bool_4 = false;
    bool bool_5 = false;
    bool bool_6 = false;
    bool bool_7 = false;
    bool bool_8 = false;

    parser.flagBool(&bool_1, "   bool-1");
    parser.flagBool(&bool_2, "bool-2");
    parser.flagBool(&bool_3, "bool-3 ");
    parser.flagBool(&bool_4, "bool-4");
    parser.flagBool(&bool_5, "bool-5   ");
    parser.flagBool(&bool_6, "  bool-6");
    parser.flagBool(&bool_7, "bool-7");
    parser.flagBool(&bool_8, "bool-8");

    auto ret = parser.parse(argc - 1, argv + 1); // TODO: This argc - 1 is stupid. Fix it.
    Check(ret);

    fmt::print("a = {}\n", a);
    fmt::print("b = {}\n", b);
    fmt::print("c = {}\n", c);
    fmt::print("d = {}\n", d);
    fmt::print("strArg = {}\n", strArg ? strArg : "null");
    fmt::print("bool_1 = {}\n", bool_1);
    fmt::print("bool_2 = {}\n", bool_2);
    fmt::print("bool_3 = {}\n", bool_3);
    fmt::print("bool_4 = {}\n", bool_4);
    fmt::print("bool_5 = {}\n", bool_5);
    fmt::print("bool_6 = {}\n", bool_6);
    fmt::print("bool_7 = {}\n", bool_7);
    fmt::print("bool_8 = {}\n", bool_8);

    return 0;
}
