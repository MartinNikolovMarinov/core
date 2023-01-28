#include "mouse.h"

mouse& mouse::set_button(key_info& button) {
    switch (button.value) {
        case GLFW_MOUSE_BUTTON_LEFT: {
            leftButton = button;
            break;
        }
        case GLFW_MOUSE_BUTTON_MIDDLE: {
            middleButton = button;
            break;
        }
        case GLFW_MOUSE_BUTTON_RIGHT: {
            rightButton = button;
            break;
        }
        default: {
            break;
        }
    }
    return *this;
}

mouse& mouse::set_scroll(i32 x, i32 y) {
    scrollX = x;
    scrollY = y;
    return *this;
}

mouse& mouse::set_pos(i32 x, i32 y) {
    this->x = x;
    this->y = y;
    return *this;
}

mouse& mouse::set_in_window(bool isInWindow) {
    this->isInWindow = isInWindow;
    return *this;
}

void mouse::clear() {
    // Mouse scrollwheels can't be held down, so we can clear the scroll on every frame.
    scrollX = 0;
    scrollY = 0;
    // Mouse buttons act the same as keyboard keys, so we do the same thing here.
    if (leftButton.is_release()) leftButton.action.type = keyboard_action::Type::NONE;
    if (middleButton.is_release()) middleButton.action.type = keyboard_action::Type::NONE;
    if (rightButton.is_release()) rightButton.action.type = keyboard_action::Type::NONE;
}

std::string mouse::to_string() {
    std::string result = "Mouse: ";
    result += '\n';
    result += "x: " + std::to_string(x) + ", ";
    result += "y: " + std::to_string(y) + ", ";
    result += '\n';
    result += "scrollX: " + std::to_string(scrollX) + ", ";
    result += "scrollY: " + std::to_string(scrollY) + ", ";
    result += '\n';
    result += "isInWindow: " + std::to_string(isInWindow) + ", ";
    result += '\n';
    result += "leftButton: " + leftButton.to_string() + ", ";
    result += '\n';
    // mouse wheel scrolling is -1 for scrolling left/up and 1 for scrolling right/down,
    result += "middleButton: " + middleButton.to_string() + ", ";
    result += '\n';
    result += "rightButton: " + rightButton.to_string() + ", ";
    result += '\n';
    return result;
}
