#include "mouse.h"

Mouse& Mouse::setButton(KeyInfo& button) {
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

Mouse& Mouse::setScroll(i32 x, i32 y) {
    scrollX = x;
    scrollY = y;
    return *this;
}

Mouse& Mouse::setPos(i32 x, i32 y) {
    this->x = x;
    this->y = y;
    return *this;
}

Mouse& Mouse::setInWindow(bool isInWindow) {
    this->isInWindow = isInWindow;
    return *this;
}

void Mouse::clear() {
    // Mouse scrollwheels can't be held down, so clear the scroll on every frame.
    scrollX = 0;
    scrollY = 0;
    // Mouse buttons act the same as keyboard keys, so do the same thing here.
    if (leftButton.isRelease()) leftButton.action.type = KeyboardAction::Type::NONE;
    if (middleButton.isRelease()) middleButton.action.type = KeyboardAction::Type::NONE;
    if (rightButton.isRelease()) rightButton.action.type = KeyboardAction::Type::NONE;
}

std::string Mouse::toString() {
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
    result += "leftButton: " + leftButton.toString() + ", ";
    result += '\n';
    // mouse wheel scrolling is -1 for scrolling left/up and 1 for scrolling right/down,
    result += "middleButton: " + middleButton.toString() + ", ";
    result += '\n';
    result += "rightButton: " + rightButton.toString() + ", ";
    result += '\n';
    return result;
}
