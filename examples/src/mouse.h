#pragma once

#include "init_core.h"

#include "keyboard.h"

#include <GLFW/glfw3.h> // FIXME: temporary include

using namespace coretypes;

namespace app {

struct Mouse {
    i32 x = 0;
    i32 y = 0;
    i32 scrollX = 0;
    i32 scrollY = 0;
    bool isInWindow = false;
    KeyInfo leftButton;
    KeyInfo middleButton;
    KeyInfo rightButton;

    Mouse& setButton(KeyInfo& button);
    Mouse& setScroll(i32 x, i32 y);
    Mouse& setPos(i32 x, i32 y);
    Mouse& setInWindow(bool isInWindow);
    void clear();
    std::string toString();
};

} // namespace app
