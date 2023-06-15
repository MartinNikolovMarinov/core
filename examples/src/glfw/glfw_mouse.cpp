#include "glfw_impl.h"

namespace app {

void setGLFWMouseButton(Mouse& m, const KeyInfo& mouseKey) {
    switch (mouseKey.value) {
        case GLFW_MOUSE_BUTTON_LEFT: {
            m.leftButton = mouseKey;
            break;
        }
        case GLFW_MOUSE_BUTTON_MIDDLE: {
            m.middleButton = mouseKey;
            break;
        }
        case GLFW_MOUSE_BUTTON_RIGHT: {
            m.rightButton = mouseKey;
            break;
        }
        default: {
            break;
        }
    }
}

} // namespace app
