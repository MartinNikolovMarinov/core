#include "ray_in_voxel_space.h"

#include <keyboard.h>
#include <shader_prog.h>
#include <grid.h>

namespace ray_in_voxel_space {

namespace {

struct State {
    static constexpr Grid2D clipSpaceGrid  = { core::v(-1.0f, -1.0f), core::v(1.0f, 1.0f) };
    static constexpr Grid2D worldSpaceGrid = { core::v(0.0f, 0.0f), core::v(1000.0f, 1000.0f) };

    ShaderProg shaderProg;

    u32 viewportWidth;
    u32 viewportHeight;

    u32 lineVBOId;
    u32 lineVAOId;
    u32 linesVertexCount;

    u32 voxelVBOId;
    u32 voxelVAOId;
    u32 voxelVertexCount;

    core::arr<bool> imgMask;
    core::vec2i imgOffset;
    core::vec2f imgDelta;
    u32 imgWidth;
    u32 imgHeight;
};

State& state(bool clear = false) {
    static State g_state = {};
    if (clear) g_state = {};
    return g_state;
}

void resetOpenGLBinds() {
    // Debug reset binds to make sure everyting is bound correctly later on.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

void drawLines(const core::mat4x4f& mvp, const core::vec4f& color) {
    State& g_s = state();
    g_s.shaderProg.use();
    g_s.shaderProg.setUniform_m("u_mvp", mvp);
    g_s.shaderProg.setUniform_v("u_color", color);
    glBindVertexArray(g_s.lineVAOId);
    glBindBuffer(GL_ARRAY_BUFFER, g_s.lineVBOId);
    glDrawArrays(GL_LINES, 0, g_s.linesVertexCount);
}

void drawVoxel(const core::mat4x4f& mvp, const core::vec4f& drawColor, const core::vec4f& fillColor) {
    State& g_s = state();
    g_s.shaderProg.use();
    g_s.shaderProg.setUniform_m("u_mvp", mvp);
    glBindVertexArray(g_s.voxelVAOId);
    glBindBuffer(GL_ARRAY_BUFFER, g_s.voxelVBOId);
    g_s.shaderProg.setUniform_v("u_color", fillColor);
    glDrawArrays(GL_TRIANGLES, 0, g_s.voxelVertexCount);
    g_s.shaderProg.setUniform_v("u_color", drawColor);
    glDrawArrays(GL_LINE_LOOP, 0, g_s.voxelVertexCount);
}

} // namespace

core::expected<GraphicsLibError> init(CommonState& s) {
    State& g_s = state();
    GLFWwindow* glfwWindow = s.mainWindow.glfwWindow;
    const char* errDesc = nullptr;

    glEnable(GL_DEPTH_TEST);

    g_s.viewportHeight = s.mainWindow.height;
    g_s.viewportWidth = s.mainWindow.width;

    glfwSetKeyCallback(glfwWindow, [](GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
        [[maybe_unused]] KeyboardModifiers keyModifiers = KeyboardModifiers::createFromGLFW(mods);
        KeyInfo keyInfo = KeyInfo::createFromGLFW(key, scancode, action);
        if (keyInfo.value == GLFW_KEY_ESCAPE && keyInfo.isPressed()) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set GLFW KeyCallback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    glfwSetFramebufferSizeCallback(glfwWindow, [](GLFWwindow*, i32 width, i32 height) {
        State& g_s = state();
        g_s.viewportWidth = width;
        g_s.viewportHeight = height;
        glViewport(0, 0, width, height);
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set GLFW FramebufferSizeCallback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    // Set Error callback:
    glfwSetErrorCallback([](i32 errCode, const char* errDesc) {
        fmt::print(stderr, "GLFW error: {}, reason: {}\n", errCode, errDesc ? errDesc : "Unknown");
    });
    if (i32 errCode = glfwGetError(&errDesc); errCode != GLFW_NO_ERROR) {
        GraphicsLibError err;
        err.code = errCode;
        err.msg = fmt::format("Failed to set GLFW Error callback, reason: {}\n", errDesc ? errDesc : "Unknown");
        return core::unexpected(core::move(err));
    }

    return {};
}

void destroy() {
    State& g_s = state();
    g_s.shaderProg.destroy();
    glDeleteBuffers(1, &g_s.lineVBOId);
    glDeleteVertexArrays(1, &g_s.lineVAOId);
    state(true);
}

core::expected<GraphicsLibError> preMainLoop(CommonState&) {
    State& g_s = state();

    // Create line drawing shader:
    {
        const char* vertexShader = R"(
            #version 330 core

            layout (location = 0) in vec3 a_pos;

            uniform mat4 u_mvp;

            void main() {
                gl_Position = u_mvp * vec4(a_pos, 1.0);
            }
        )";
        const char* fragmentShader = R"(
            #version 330 core

            out vec4 fragColor;

            uniform vec4 u_color;

            void main() {
                fragColor = u_color;
            }
        )";
        g_s.shaderProg = ValueOrDie(ShaderProg::create(vertexShader, fragmentShader));
    }

    // Create line vertices:
    {
        core::arr<core::vec3f> lineVertices;
        lineVertices.append(core::v(-1.0f, 1.0f, 0.0f));
        lineVertices.append(core::v(1.0f, -1.0f, 0.0f));

        g_s.linesVertexCount = lineVertices.len();

        // Create VBO:
        glGenBuffers(1, &g_s.lineVBOId);
        glBindBuffer(GL_ARRAY_BUFFER, g_s.lineVBOId);
        ptr_size bufferDataSize = lineVertices.len() * sizeof(decltype(lineVertices)::data_type);
        glBufferData(GL_ARRAY_BUFFER, bufferDataSize, lineVertices.data(), GL_STATIC_DRAW);

        // Create VAO:
        glGenVertexArrays(1, &g_s.lineVAOId);
        glBindVertexArray(g_s.lineVAOId);

        // Link vertex attributes:
        constexpr ptr_size stride = sizeof(decltype(lineVertices)::data_type);
        constexpr ptr_size dimensions = decltype(lineVertices)::data_type::dimensions();
        u32 posAttribLoc = ValueOrDie(g_s.shaderProg.getAttribLocation("a_pos"));
        glVertexAttribPointer(posAttribLoc, dimensions, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(posAttribLoc);
    }

    // Create quad vertices:
    {
        core::arr<core::vec3f> quadVertices;
        quadVertices.append(core::v(-1.0f, 1.0f, 0.0f));
        quadVertices.append(core::v(1.0f, 1.0f, 0.0f));
        quadVertices.append(core::v(1.0f, -1.0f, 0.0f));
        quadVertices.append(core::v(-1.0f, -1.0f, 0.0f));
        quadVertices.append(core::v(-1.0f, 1.0f, 0.0f));
        quadVertices.append(core::v(1.0f, -1.0f, 0.0f));

        g_s.voxelVertexCount = quadVertices.len();

        // Create VBO:
        glGenBuffers(1, &g_s.voxelVBOId);
        glBindBuffer(GL_ARRAY_BUFFER, g_s.voxelVBOId);
        ptr_size bufferDataSize = quadVertices.len() * sizeof(decltype(quadVertices)::data_type);
        glBufferData(GL_ARRAY_BUFFER, bufferDataSize, quadVertices.data(), GL_STATIC_DRAW);

        // Create VAO:
        glGenVertexArrays(1, &g_s.voxelVAOId);
        glBindVertexArray(g_s.voxelVAOId);

        // Link vertex attributes:
        constexpr ptr_size stride = sizeof(decltype(quadVertices)::data_type);
        constexpr ptr_size dimensions = decltype(quadVertices)::data_type::dimensions();
        u32 posAttribLoc = ValueOrDie(g_s.shaderProg.getAttribLocation("a_pos"));
        glVertexAttribPointer(posAttribLoc, dimensions, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(posAttribLoc);
    }

    // Create voxel grid
    {
        g_s.imgOffset = core::v(200, 200);
        g_s.imgDelta = core::v(20.f, 20.f);
        g_s.imgWidth = 8;
        g_s.imgHeight = 8;
        g_s.imgMask = core::arr<bool>(g_s.imgWidth * g_s.imgHeight, g_s.imgWidth * g_s.imgHeight);
        for (u32 y = 0; y < g_s.imgHeight; ++y) {
            for (u32 x = 0; x < g_s.imgWidth; ++x) {
                g_s.imgMask[y * g_s.imgWidth + x] = (x + y) % 2 == 0;
            }
        }
    }

    return {};
}

void mainLoop(CommonState& commonState) {
    State& g_s = state();
    [[maybe_unused]] auto rotationAxisZ = core::v(0.0f, 0.0f, -1.0f);

    resetOpenGLBinds();
    {
        auto model = core::mat4x4f::identity();
        auto view = core::mat4x4f::identity();
        auto projection = core::mat4x4f::identity();
        auto mvp = projection * view * model;
        drawLines(mvp, core::RED);
    }

    resetOpenGLBinds();
    {
        // auto model = core::mat4x4f::identity();
        // core::translate(model, core::v(0.2f, 0.2f, 0.0f));
        // auto view = core::mat4x4f::identity();
        // auto projection = core::mat4x4f::identity();
        // auto mvp = projection * view * model;
        // drawVoxel(mvp, core::BLACK, core::WHITE);

        auto projection = core::mat4x4f::identity(); // TODO: might want to move outside the main loop.

        auto model = core::mat4x4f::identity();
        auto tmp = core::v(200.f, 200.f, 1.0f);
        core::translate(model, tmp);
        auto view = g_s.worldSpaceGrid.conversionMatrix(g_s.clipSpaceGrid);
        auto mvp = projection * view * model;
        drawVoxel(mvp, core::BLACK, core::WHITE);

        // for (u32 y = 0; y < g_s.imgHeight; ++y) {
        //     for (u32 x = 0; x < g_s.imgWidth; ++x) {
        //         if (g_s.imgMask[y * g_s.imgWidth + x]) {
        //             core::vec2f xy = core::v(
        //                 g_s.imgOffset.x() + f32(x) * g_s.imgDelta.x(),
        //                 g_s.imgOffset.y() + f32(y) * g_s.imgDelta.y()
        //             );
        //             xy = convertVecUsingGrid(xy, g_s.worldSpaceGrid, g_s.clipSpaceGrid);
        //             auto model = core::mat4x4f::identity();
        //             core::translate(model, core::v(xy.x(), xy.y(), 0.0f));
        //             auto view = core::mat4x4f::identity();
        //             auto mvp = projection * view * model;
        //             drawVoxel(mvp, core::BLACK, core::WHITE);
        //         }
        //     }
        // }
    }

    glfwSwapBuffers(commonState.mainWindow.glfwWindow);
    fmt::print("Frame: {}, FPS: {:f}\n", commonState.frameCount, commonState.fps);
}

} // namespace ray_in_voxel_space

