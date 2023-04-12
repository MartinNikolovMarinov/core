#include "raytracing.h"

#include <keyboard.h>
#include <shader_prog.h>

#include <memory>

namespace raytracing {

namespace {

struct State {
    ShaderProg shaderProg;

    u32 textureId;
    u32 quadVBOId;
    u32 quadEBOId;
    u32 quadVAOId;
    u32 quadIndicesCount;
    u32 textureColorVBOId;
};

State& state(bool clear = false) {
    static State g_state = {};
    if (clear) g_state = {};
    return g_state;
}

void writeColor(u8* data, const core::vec3f& color) {
    data[0] = static_cast<u8>(color.r() * 255.999f);
    data[1] = static_cast<u8>(color.g() * 255.999f);
    data[2] = static_cast<u8>(color.b() * 255.999f);
}

struct ray {
    core::vec3f origin;
    core::vec3f direction;

    core::vec3f at(f32 t) const {
        return origin + t * direction;
    }
};

struct hitRecord {
    core::vec3f p;
    core::vec3f normal;
    f32 t;
    bool frontFace;

    inline void setFaceNormal(const ray& r, const core::vec3f& outwardNormal) {
        frontFace = core::vdot(r.direction, outwardNormal) < 0;
        normal = frontFace ? outwardNormal : -outwardNormal;
    }
};

class hittable {
public:
    virtual ~hittable() = default;
    virtual bool hit(const ray& r, f32 tMin, f32 tMax, hitRecord& rec) const = 0;
};

class hittableList : public hittable {
public:
    void clear() { m_objects.clear(); }
    hittableList& add(std::shared_ptr<hittable> object) { m_objects.append(object); return *this; }

    virtual bool hit(const ray& r, f32 tMin, f32 tMax, hitRecord& rec) const override {
        hitRecord tempRec;
        bool hitAnything = false;
        auto closestSoFar = tMax;

        for (i32 i = 0; i < m_objects.len(); ++i) {
            const auto& object = m_objects[i];
            if (object->hit(r, tMin, closestSoFar, tempRec)) {
                hitAnything = true;
                closestSoFar = tempRec.t;
                rec = tempRec;
            }
        }

        return hitAnything;
    }

private:
    core::arr<std::shared_ptr<hittable>> m_objects;
};

class sphere : public hittable {
public:
    core::vec3f m_center;
    f32 m_radius;

    sphere() = default;
    sphere(const core::vec3f& center, f32 radius) : m_center(center), m_radius(radius) {}

    virtual bool hit(const ray& r, f32 tMin, f32 tMax, hitRecord& rec) const override {
        core::vec3f oc = r.origin - m_center;
        auto a = core::vlengthsq(r.direction);
        auto halfB = core::vdot(oc, r.direction);
        auto c = core::vlengthsq(oc) - m_radius * m_radius;
        auto discriminant = halfB * halfB - a * c;
        if (discriminant < 0) {
            return false;
        }
        auto sqrtd = std::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (-halfB - sqrtd) / a;
        if (root < tMin || tMax < root) {
            root = (-halfB + sqrtd) / a;
            if (root < tMin || tMax < root) {
                return false;
            }
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        core::vec3f outwardNormal = (rec.p - m_center) / m_radius;
        rec.setFaceNormal(r, outwardNormal);
        return true;
    }
};

f32 hitSphere(const core::vec3f& center, double radius, const ray& r) {
    core::vec3f oc = r.origin - center;
    auto a = core::vlengthsq(r.direction);
    auto halfB = core::vdot(oc, r.direction);
    auto c = core::vlengthsq(oc) - radius * radius;
    auto discriminant = halfB * halfB - a * c;
    if (discriminant < 0) {
        return -1.0f;
    }
    return (-halfB - std::sqrt(discriminant)) / a;
}

core::vec3f rayColor(const ray& r, const hittable& world) {
    hitRecord rec;
    if (world.hit(r, 0, INFINITY, rec)) {
        return 0.5f * (rec.normal + core::v(1.f, 1.f, 1.f));
    }
    core::vec3f unitDirection = core::vnorm(r.direction);
    auto t = 0.5f * (unitDirection.y() + 1.0f);
    constexpr auto colorA = core::v(1.0f, 1.0f, 1.0f);
    constexpr auto colorB = core::v(0.5f, 0.7f, 1.0f);
    return core::lerp(colorA, colorB, t);
}

}

core::expected<GraphicsLibError> init(CommonState& s) {
    GLFWwindow* glfwWindow = s.mainWindow.glfwWindow;
    const char* errDesc = nullptr;

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
    state(true);
}

core::expected<GraphicsLibError> preMainLoop(CommonState&) {
    State& g_s = state();

    // Create shader program:
    {
        const char* vertexShaderSource = R"(
            #version 330 core

            layout (location = 0) in vec3 a_pos;
            layout (location = 1) in vec2 a_texCoord;

            out vec2 texCoord;

            void main() {
                gl_Position = vec4(a_pos, 1.0);
                texCoord = a_texCoord;
            }
        )";
        const char* fragmentShaderSource = R"(
            #version 330 core

            out vec4 fragColor;

            in vec2 texCoord;

            uniform sampler2D u_texture;

            void main() {
                fragColor = texture(u_texture, texCoord);
            }
        )";
        g_s.shaderProg = ValueOrDie(ShaderProg::create(vertexShaderSource, fragmentShaderSource));
    }

    // Create the quad that will fill the entire screen
    {
        f32 vertices[] = {
            // positions        // texture coords
            1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
            1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
        };

        u32 indices[] = {
            0, 1, 3,
            1, 2, 3,
        };

        g_s.quadIndicesCount = sizeof(indices) / sizeof(indices[0]);

        // Create VBO, bind it and store the vertex buffer data:
        glGenBuffers(1, &g_s.quadVBOId);
        glBindBuffer(GL_ARRAY_BUFFER, g_s.quadVBOId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Create EBO, bind it and store the index buffer data:
        glGenBuffers(1, &g_s.quadEBOId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_s.quadEBOId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Create VAO and bind it:
        glGenVertexArrays(1, &g_s.quadVAOId);
        glBindVertexArray(g_s.quadVAOId);

        // Link vertex attributes:
        auto posAttribLoc = ValueOrDie(g_s.shaderProg.getAttribLocation("a_pos"));
        glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)0);
        glEnableVertexAttribArray(0);
        auto texCoordAttribLoc = ValueOrDie(g_s.shaderProg.getAttribLocation("a_texCoord"));
        glVertexAttribPointer(texCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)(3 * sizeof(f32)));
        glEnableVertexAttribArray(1);
    }

    // Create the texture for the screen quad
    {
        // TODO: I kinda want to recalculate this on window resize. It can't be staticly drawn.

        glGenTextures(1, &g_s.textureId);
        glBindTexture(GL_TEXTURE_2D, g_s.textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Image
        constexpr f32 aspectRatio = 16.0f / 9.0f;
        constexpr u32 imageWidth = 400;
        constexpr u32 imageHeight = u32(f32(imageWidth) / aspectRatio);

        // World
        hittableList world;
        world.add(std::make_shared<sphere>(core::v(0.0f, 0.0f, -1.0f), 0.5f));
        world.add(std::make_shared<sphere>(core::v(0.0f, -100.5f, -1.0f), 100.0f));

        // Camera
        constexpr f32 viewportHeight = 2.0f;
        constexpr f32 viewportWidth = aspectRatio * viewportHeight;
        constexpr f32 focalLength = 1.0f;

        constexpr core::vec3f origin = core::v(0.0f, 0.0f, 0.0f);
        constexpr core::vec3f horizontal = core::v(viewportWidth, 0.0f, 0.0f);
        constexpr core::vec3f vertical = core::v(0.0f, viewportHeight, 0.0f);
        constexpr core::vec3f lowerLeftCorner = origin - horizontal / 2.0f - vertical / 2.0f - core::v(0.0f, 0.0f, focalLength);

        // Render raytraced scene:
        constexpr u32 nchannels = 3;
        constexpr u32 pixelCount = imageWidth * imageHeight * nchannels;
        u8 pixelData[pixelCount];
        for (u32 j = 0; j < imageHeight; ++j) {
            for (u32 i = 0; i < imageWidth; ++i) {
                const f32 u = f32(i) / f32(imageWidth - 1);
                const f32 v = f32(j) / f32(imageHeight - 1);
                ray r;
                r.origin = origin;
                r.direction = lowerLeftCorner + u * horizontal + v * vertical - origin;
                core::vec3f color = rayColor(r, world);

                i32 offset = i * nchannels + j * imageWidth * nchannels;
                writeColor(pixelData + offset, color);
            }
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pixelData);
        glGenerateMipmap(GL_TEXTURE_2D);
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

} // namespace raytracing
