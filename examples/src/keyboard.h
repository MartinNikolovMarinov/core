#pragma once

#include "init_core.h"

#include <GLFW/glfw3.h>

#include <string>

using namespace coretypes;

struct KeyboardAction {
    enum Type : u8 {
        NONE = 0,
        KEY_PRESS = 1,
        KEY_RELEASE = 2,
        KEY_REPEAT = 3,

        SENTINEL
    };

    static KeyboardAction createFromGLFW(i32 action);

    Type type = Type::NONE;

    bool operator==(const KeyboardAction& other) const;

    i32 toGLFWAction();
    const char* toCptr();
};

struct KeyboardModifiers {
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

    static KeyboardModifiers createFromGLFW(i32 mods);

    Type type = Type::NONE;

    i32 toGLFWMods();
    const char* toCptr();
};

struct KeyInfo {
    static KeyInfo createFromGLFW(i32 key, i32 scancode, i32 action);

    i32 value = 0; // TODO2: Don't use i32. Translate this to a concreate type.
    i32 scancode = 0;
    KeyboardAction action = { KeyboardAction::Type::NONE };

    bool operator==(const KeyInfo& other) const;

    bool isPressed();
    bool isRelease();
    bool isRepeat();
    std::string toString();
};

struct Keyboard {
    KeyInfo keys[GLFW_KEY_LAST] = {};
    KeyboardModifiers modifiers = { KeyboardModifiers::Type::NONE };
    core::rune textInput;

    Keyboard& setKey(KeyInfo&& key);
    KeyInfo& getKey(i32 scancode);

    Keyboard& setModifiers(KeyboardModifiers&& mods);
    KeyboardModifiers& getModifiers();

    Keyboard& setTextInput(core::rune&& text);
    core::rune& getTextInput();

    void clear();

    std::string toString();
};
