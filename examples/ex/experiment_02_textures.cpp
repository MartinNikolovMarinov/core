#include "experiment_02_textures.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <keyboard.h>
#include <shader_prog.h>
#include <mouse.h>

#include <glfw/glfw_impl.h>

namespace textures_ex_02 {

namespace {

struct State {
    ShaderProg shaderProg;

    u32 textureId;
    u32 quadVBOId;
    u32 quadVAOId;
    u32 quadEBOId;
    u32 quadIndicesCount;
    u32 textureColorVBOId;
};

State& state(bool clear = false) {
    static State g_state = {};
    if (clear) g_state = {};
    return g_state;
}

} // namespace

core::expected<GraphicsLibError> init(CommonState& s) {
    GLFWwindow* glfwWindow = s.mainWindow.glfwWindow;
    const char* errDesc = nullptr;

    glfwSetKeyCallback(glfwWindow, [](GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
        [[maybe_unused]] app::KeyboardModifiers keyModifiers = app::createKeyboardModifiersGLFW(mods);
        app::KeyInfo keyInfo = app::createKeyInfoGLFW(key, scancode, action);
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
    glDeleteBuffers(1, &g_s.quadVBOId);
    glDeleteVertexArrays(1, &g_s.quadVAOId);
    glDeleteBuffers(1, &g_s.quadEBOId);
    glDeleteBuffers(1, &g_s.textureColorVBOId);
    glDeleteTextures(1, &g_s.textureId);
    g_s.shaderProg.destroy();
    state(true);
}

core::expected<GraphicsLibError> preMainLoop(CommonState&) {
    State& g_s = state();

    // Create shader program:
    {
        const char* vertexShaderSource = R"(
            #version 330 core

            layout (location = 0) in vec2 a_pos;
            layout (location = 1) in vec2 a_texCoord;
            layout (location = 2) in vec3 a_texColor;

            out vec3 texColor;
            out vec2 texCoord;

            void main() {
                gl_Position = vec4(a_pos.xy, 1.0, 1.0);
                texColor = a_texColor;
                texCoord = a_texCoord;
            }
        )";
        const char* fragShaderSource = R"(
            #version 330 core

            out vec4 fragColor;

            in vec3 texColor;
            in vec2 texCoord;

            uniform sampler2D u_texture;

            void main() {
                fragColor = texture(u_texture, texCoord) * vec4(texColor.xyz, 1.0);
            }
        )";
        g_s.shaderProg = ValueOrDie(ShaderProg::create(vertexShaderSource, fragShaderSource));
    }

    // Create quad
    {
        // Create vertices:
        core::arr<core::vec2f> vertices;
        vertices.append({0.5f,  0.5f})
                .append({0.5f, -0.5f})
                .append({-0.5f, -0.5f})
                .append({-0.5f,  0.5f});

        glGenBuffers(1, &g_s.quadVBOId);
        glBindBuffer(GL_ARRAY_BUFFER, g_s.quadVBOId);
        glBufferData(GL_ARRAY_BUFFER, vertices.len() * sizeof(core::vec2f), vertices.data(), GL_STATIC_DRAW);

        // Create indices:
        core::arr<u32> indices;
        indices.append(0).append(1).append(3)  // triangle 1
               .append(1).append(2).append(3); // trinagle 2

        g_s.quadIndicesCount = indices.len();

        glGenBuffers(1, &g_s.quadEBOId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_s.quadEBOId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.len() * sizeof(u32), indices.data(), GL_STATIC_DRAW);

        // Create VAO and link vertex attributes:
        glGenVertexArrays(1, &g_s.quadVAOId);
        glBindVertexArray(g_s.quadVAOId);
        u32 posLoc = ValueOrDie(g_s.shaderProg.getAttribLocation("a_pos"));
        constexpr u32 stride = sizeof(core::vec2f);
        constexpr u32 dimensions = core::vec2f::dimensions();
        glVertexAttribPointer(posLoc, dimensions, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(posLoc);
    }

    // Load image and create texture:
    {
        glBindVertexArray(g_s.quadVAOId); // not needed but just incase I re-areange the code later.

        // Load the texture:
        i32 width, height, nrChannels;
        constexpr const char* containerJPGPath = PATH_TO_DATA"/container.jpg";
        unsigned char *data = stbi_load(containerJPGPath, &width, &height, &nrChannels, 0);
        if (!data) {
            GraphicsLibError err;
            err.code = 0;
            err.msg = fmt::format("Failed to load texture from path: {}\n", containerJPGPath);
            return core::unexpected(core::move(err));
        }
        // after after creating a OpenGL texture out of the image it can be freed:
        defer { stbi_image_free(data); };

        glGenTextures(1, &g_s.textureId);
        glBindTexture(GL_TEXTURE_2D, g_s.textureId);
        // Set the texture parameters:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Generate the texture:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Create texture coordinates:
        core::arr<core::vec2f> texCoords;
        texCoords.append({1.0f, 1.0f});
        texCoords.append({1.0f, 0.0f});
        texCoords.append({0.0f, 0.0f});
        texCoords.append({0.0f, 1.0f});

        // Link texture coordinates attributes:
        u32 posLoc = ValueOrDie(g_s.shaderProg.getAttribLocation("a_texCoord"));
        constexpr u32 stride = sizeof(core::vec2f);
        constexpr u32 dimensions = core::vec2f::dimensions();
        glVertexAttribPointer(posLoc, dimensions, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(posLoc);
    }

    // Create colors
    {
        core::arr<core::vec3f> colors;
        colors.append({1.0f, 0.0f, 0.0f})
              .append({0.0f, 1.0f, 0.0f})
              .append({0.0f, 0.0f, 1.0f})
              .append({1.0f, 1.0f, 0.0f});

        glGenBuffers(1, &g_s.textureColorVBOId);
        glBindBuffer(GL_ARRAY_BUFFER, g_s.textureColorVBOId);
        glBufferData(GL_ARRAY_BUFFER, colors.len() * sizeof(core::vec3f), colors.data(), GL_STATIC_DRAW);

        u32 posLoc = ValueOrDie(g_s.shaderProg.getAttribLocation("a_texColor"));
        constexpr u32 stride = sizeof(core::vec3f);
        constexpr u32 dimensions = core::vec3f::dimensions();
        glVertexAttribPointer(posLoc, dimensions, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(posLoc);
    }

    return {};
}

void mainLoop(CommonState& commonState) {
    State& g_s = state();

    // Debug reset binds to make sure everyting is bound correctly later on.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);

    g_s.shaderProg.use();

    glBindVertexArray(g_s.quadVAOId);
    glBindBuffer(GL_ARRAY_BUFFER, g_s.quadVBOId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_s.quadEBOId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_s.textureId);
    glDrawElements(GL_TRIANGLES, g_s.quadIndicesCount, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(commonState.mainWindow.glfwWindow);
    fmt::print("Frame: {}, FPS: {:f}\n", commonState.frameCount, commonState.fps);
}

} // namespace textures_ex_02
