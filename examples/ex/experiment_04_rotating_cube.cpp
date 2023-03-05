#include "experiment_04_rotating_cube.h"

#include <keyboard.h>
#include <shader_prog.h>

namespace rotating_cube_ex_04 {

namespace {

struct State {
    ShaderProg shaderProg;

    i32 viewportWidth;
    i32 viewportHeight;

    u32 cubeVBOId;
    u32 cubeVAOId;
    u32 cubeVertCount;
    core::vec4f cubeColor;
};

State& state(bool clear = false) {
    static State g_state = {};
    if (clear) g_state = {};
    return g_state;
}

} // namespace

core::expected<GraphicsLibError> init(CommonState& s) {
    State& g_s = state();
    GLFWwindow* glfwWindow = s.mainWindow.glfwWindow;
    const char* errDesc = nullptr;

    g_s.viewportWidth = s.mainWindow.width;
    g_s.viewportHeight = s.mainWindow.height;

    glEnable(GL_DEPTH_TEST);

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
    glDeleteBuffers(1, &g_s.cubeVBOId);
    glDeleteVertexArrays(1, &g_s.cubeVAOId);
    g_s.shaderProg.destroy();
    state(true);
}

core::expected<GraphicsLibError> preMainLoop(CommonState&) {
    State& g_s = state();

    // Create shader program:
    {
        const char* vertexShaderSource = R"(
            #version 330 core

            layout (location = 0) in vec3 in_pos;

            uniform mat4 u_projection;
            uniform mat4 u_view;
            uniform mat4 u_model;

            void main() {
                gl_Position = u_projection * u_view * u_model * vec4(in_pos.xyz, 1.0);
            }
        )";
        const char* fragShaderSource = R"(
            #version 330 core

            out vec4 fragColor;
            uniform vec4 u_color;

            void main() {
                fragColor = u_color;
            }
        )";
        g_s.shaderProg = ValueOrDie(ShaderProg::create(vertexShaderSource, fragShaderSource));
    }

    // Create cube:
    {
        g_s.cubeColor = core::v(0.2f, 0.7f, 0.5f, 1.0f);

        core::arr<core::vec3f> vertices(0, 6*6);
        vertices.append(core::v(-0.5f, -0.5f, -0.5f))
                .append(core::v(0.5f, -0.5f, -0.5f))
                .append(core::v(0.5f,  0.5f, -0.5f))
                .append(core::v(0.5f,  0.5f, -0.5f))
                .append(core::v(-0.5f,  0.5f, -0.5f))
                .append(core::v(-0.5f, -0.5f, -0.5f));

        vertices.append(core::v(-0.5f, -0.5f,  0.5f))
                .append(core::v(0.5f, -0.5f,  0.5f))
                .append(core::v(0.5f,  0.5f,  0.5f))
                .append(core::v(0.5f,  0.5f,  0.5f))
                .append(core::v(-0.5f,  0.5f,  0.5f))
                .append(core::v(-0.5f, -0.5f,  0.5f));

        vertices.append(core::v(-0.5f,  0.5f,  0.5f))
                .append(core::v(-0.5f,  0.5f, -0.5f))
                .append(core::v(-0.5f, -0.5f, -0.5f))
                .append(core::v(-0.5f, -0.5f, -0.5f))
                .append(core::v(-0.5f, -0.5f,  0.5f))
                .append(core::v(-0.5f,  0.5f,  0.5f));

        vertices.append(core::v(0.5f,  0.5f,  0.5f))
                .append(core::v(0.5f,  0.5f, -0.5f))
                .append(core::v(0.5f, -0.5f, -0.5f))
                .append(core::v(0.5f, -0.5f, -0.5f))
                .append(core::v(0.5f, -0.5f,  0.5f))
                .append(core::v(0.5f,  0.5f,  0.5f));

        vertices.append(core::v(-0.5f, -0.5f, -0.5f))
                .append(core::v(0.5f, -0.5f, -0.5f))
                .append(core::v(0.5f, -0.5f,  0.5f))
                .append(core::v(0.5f, -0.5f,  0.5f))
                .append(core::v(-0.5f, -0.5f,  0.5f))
                .append(core::v(-0.5f, -0.5f, -0.5f));

        vertices.append(core::v(-0.5f,  0.5f, -0.5f))
                .append(core::v(0.5f,  0.5f, -0.5f))
                .append(core::v(0.5f,  0.5f,  0.5f))
                .append(core::v(0.5f,  0.5f,  0.5f))
                .append(core::v(-0.5f,  0.5f,  0.5f))
                .append(core::v(-0.5f,  0.5f, -0.5f));

        g_s.cubeVertCount = vertices.len();

        // Create VBO, bind it and store the vertex buffer data:
        glGenBuffers(1, &g_s.cubeVBOId);
        glBindBuffer(GL_ARRAY_BUFFER, g_s.cubeVBOId);
        ptr_size bufferDataSize = vertices.len() * sizeof(core::vec3f);
        glBufferData(GL_ARRAY_BUFFER, bufferDataSize, vertices.data(), GL_STATIC_DRAW);

        // Create VAO and bind it:
        glGenVertexArrays(1, &g_s.cubeVAOId);
        glBindVertexArray(g_s.cubeVAOId);

        // Link vertex attributes:
        constexpr ptr_size stride = sizeof(core::vec3f);
        constexpr ptr_size dimensions = core::vec3f::dimensions();
        constexpr u32 inPosAttribLocation = 0;
        glVertexAttribPointer(inPosAttribLocation, dimensions, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(inPosAttribLocation);
    }

    return {};
}

void mainLoop(CommonState& commonState) {
    State& g_s = state();
    auto timeSiceStart_seconds = glfwGetTime();

    // Debug reset binds to make sure everyting is bound correctly later on.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);

    auto model = core::mat4x4f::identity();
    core::rotate_right(model, core::v(0.5f, 1.0f, 0.0f), core::deg_to_rad(50.0f) * (f32)timeSiceStart_seconds);

    auto view = core::mat4x4f::identity();
    core::translate(view, core::v(0.0f, 0.0f, -3.0f));

    auto projection = core::mat4x4f::identity();
    projection = core::perspective(core::deg_to_rad(45.0f), (f32)g_s.viewportWidth / (f32)g_s.viewportHeight, 0.1f, 100.0f);

    g_s.shaderProg.use();
    g_s.shaderProg.setUniform_v("u_color", g_s.cubeColor);
    g_s.shaderProg.setUniform_m("u_model", model);
    g_s.shaderProg.setUniform_m("u_view", view);
    g_s.shaderProg.setUniform_m("u_projection", projection);

    glBindVertexArray(g_s.cubeVAOId);
    glBindBuffer(GL_ARRAY_BUFFER, g_s.cubeVBOId);
    glDrawArrays(GL_TRIANGLES, 0, g_s.cubeVertCount);

    glfwSwapBuffers(commonState.mainWindow.glfwWindow);
    fmt::print("Frame: {}, FPS: {:f}\n", commonState.frameCount, commonState.fps);
}

} // namespace transformation_ex_03
