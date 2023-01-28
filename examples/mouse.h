#pragma once

#include "keyboard.h"

#include <core.h>

using namespace coretypes;

struct mouse {
    i32 x = 0;
    i32 y = 0;
    i32 scrollX = 0;
    i32 scrollY = 0;
    bool isInWindow = false;
    key_info leftButton;
    key_info middleButton;
    key_info rightButton;

    mouse& set_button(key_info& button);
    mouse& set_scroll(i32 x, i32 y);
    mouse& set_pos(i32 x, i32 y);
    mouse& set_in_window(bool isInWindow);
    void clear();
    std::string to_string();
};
