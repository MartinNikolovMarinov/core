#pragma once

#include "init_core.h"

#include "keyboard.h"

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

    Mouse& setScroll(i32 x, i32 y);
    Mouse& setPos(i32 x, i32 y);
    Mouse& setInWindow(bool isInWindow);
    void update();
    std::string toString();
};

} // namespace app
