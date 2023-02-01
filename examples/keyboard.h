#pragma once

#include "init_core.h"

#include <GLFW/glfw3.h>

#include <string>

using namespace coretypes;

struct keyboard_action {
    enum Type : u8 {
        NONE = 0,
        KEY_PRESS = 1,
        KEY_RELEASE = 2,
        KEY_REPEAT = 3,

        SENTINEL
    };

    static keyboard_action create_from_glfw(i32 action);

    Type type = Type::NONE;

    bool operator==(const keyboard_action& other) const;

    i32 to_glfw_action();
    const char* to_cptr();
};

struct keyboard_modifiers {
    enum Type : i8 {
        NONE = 0,
        SHIFT = 1 << 0,
        CONTROL = 1 << 1,
        ALT = 1 << 2,
        SUPER = 1 << 3,
        CAPS_LOCK = 1 << 4,
        NUM_LOCK = 1 << 5,

        SENTINEL
    };

    static keyboard_modifiers create_from_glfw(i32 mods);

    Type type = Type::NONE;

    i32 to_glfw_mods();
    const char* to_cptr();
};

struct key_info {
    static key_info create_from_glfw(i32 key, i32 scancode, i32 action);

    i32 value = 0; // TODO: Don't use i32. Translate this to a concreate type.
    i32 scancode = 0;
    keyboard_action action = { keyboard_action::Type::NONE };

    bool operator==(const key_info& other) const;

    bool is_pressed();
    bool is_release();
    bool is_repeat();
    std::string to_string();
};

struct keyboard {
    key_info keys[GLFW_KEY_LAST] = {};
    keyboard_modifiers modifiers = { keyboard_modifiers::Type::NONE };
    core::rune textInput;

    keyboard& set_key(key_info&& key);
    key_info& get_key(i32 scancode);

    keyboard& set_modifiers(keyboard_modifiers&& mods);
    keyboard_modifiers& get_modifiers();

    keyboard& set_text_input(core::rune&& text);
    core::rune& get_text_input();

    void clear();

    std::string to_string();
};
