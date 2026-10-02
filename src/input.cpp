#pragma once

#include <cassert>

#include "common.h"
#include "gmath.h"

enum class InputState
{
    Up = 0,
    Down,
};

enum class Key
{
    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

    Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,

    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

    Backspace, Tab, Enter, Space, Escape, Delete,
    Right, Left, Down, Up,
    LeftCtrl, LeftShift, LeftAlt,
    RightCtrl, RightShift, RightAlt,

    Count
};

enum class MouseButton
{
    Left,
    Right,
    Middle,

    Count
};

enum class GamepadButton
{
    A, B, X, Y,
    Back, Start,
    LeftShoulder, RightShoulder,
    DpadUp, DpadDown, DpadLeft, DpadRight,
    LeftStick, RightStick,

    Count
};


class GameInput {
public:
    static constexpr s32 MaxGamepads         = 4;
    static constexpr s32 KeyCount            = static_cast<s32>(Key::Count);
    static constexpr s32 MouseButtonCount    = static_cast<s32>(MouseButton::Count);
    static constexpr s32 GamepadButtonCount  = static_cast<s32>(GamepadButton::Count);


    struct Keyboard {
        InputState keys[KeyCount] = {};
    };

    struct Mouse {
        InputState buttons[MouseButtonCount] = {};
        s32 scrollWheel = 0;
        f32 x = 0.0f;
        f32 y = 0.0f;
    };

    struct Gamepad {
        b32 connected = false;
        InputState buttons[GamepadButtonCount] = {};

        f32 leftStickX   = 0.0f;
        f32 leftStickY   = 0.0f;
        f32 rightStickX  = 0.0f;
        f32 rightStickY  = 0.0f;

        f32 leftTrigger  = 0.0f;
        f32 rightTrigger = 0.0f;
    };

    Keyboard keyboard;
    Mouse    mouse;
    Gamepad  gamepads[MaxGamepads];

    Keyboard previousKeyboard;
    Mouse    previousMouse;
    Gamepad  previousGamepads[MaxGamepads];

    GameInput() = default;

    void beginFrame()
    {
        previousKeyboard = keyboard;
        previousMouse    = mouse;

        for (s32 i = 0; i < MaxGamepads; ++i)
        {
            previousGamepads[i] = gamepads[i];
        }
    }

    void reset()
    {
        *this = GameInput();
    }

    b32 isKeyDown(Key key) const     { return keyboard.keys[toIndex(key)] == InputState::Down; }
    b32 isKeyUp(Key key) const       { return keyboard.keys[toIndex(key)] == InputState::Up; }
    b32 isKeyPressed(Key key) const  { return wasPressed(keyboard.keys[toIndex(key)], previousKeyboard.keys[toIndex(key)]); }
    b32 isKeyReleased(Key key) const { return wasReleased(keyboard.keys[toIndex(key)], previousKeyboard.keys[toIndex(key)]); }

    void setKeyState(Key key, InputState state) { keyboard.keys[toIndex(key)] = state; }

    b32 isMouseButtonDown(MouseButton button) const     { return mouse.buttons[toIndex(button)] == InputState::Down; }
    b32 isMouseButtonUp(MouseButton button) const       { return mouse.buttons[toIndex(button)] == InputState::Up; }
    b32 isMouseButtonPressed(MouseButton button) const  { return wasPressed(mouse.buttons[toIndex(button)], previousMouse.buttons[toIndex(button)]); }
    b32 isMouseButtonReleased(MouseButton button) const { return wasReleased(mouse.buttons[toIndex(button)], previousMouse.buttons[toIndex(button)]); }

    void setMouseButton(MouseButton button, InputState state) { mouse.buttons[toIndex(button)] = state; }

    b32 isLeftMouseDown() const       { return isMouseButtonDown(MouseButton::Left); }
    b32 isLeftMouseUp() const         { return isMouseButtonUp(MouseButton::Left); }
    b32 isLeftMousePressed() const    { return isMouseButtonPressed(MouseButton::Left); }
    b32 isLeftMouseReleased() const   { return isMouseButtonReleased(MouseButton::Left); }

    b32 isRightMouseDown() const      { return isMouseButtonDown(MouseButton::Right); }
    b32 isRightMouseUp() const        { return isMouseButtonUp(MouseButton::Right); }
    b32 isRightMousePressed() const   { return isMouseButtonPressed(MouseButton::Right); }
    b32 isRightMouseReleased() const  { return isMouseButtonReleased(MouseButton::Right); }

    b32 isMiddleMouseDown() const     { return isMouseButtonDown(MouseButton::Middle); }
    b32 isMiddleMouseUp() const       { return isMouseButtonUp(MouseButton::Middle); }
    b32 isMiddleMousePressed() const  { return isMouseButtonPressed(MouseButton::Middle); }
    b32 isMiddleMouseReleased() const { return isMouseButtonReleased(MouseButton::Middle); }

    void setLeftMouse(InputState state)   { setMouseButton(MouseButton::Left, state); }
    void setRightMouse(InputState state)  { setMouseButton(MouseButton::Right, state); }
    void setMiddleMouse(InputState state) { setMouseButton(MouseButton::Middle, state); }

    void setMousePosition(f64 x, f64 y)
    {
        mouse.x = static_cast<f32>(x);
        mouse.y = static_cast<f32>(y);
    }

    vec2 mousePosition() const { return vec2{mouse.x, mouse.y}; }
    vec2 mouseDelta() const    { return vec2{mouse.x - previousMouse.x, mouse.y - previousMouse.y}; }

    void addScroll(s32 amount)      { mouse.scrollWheel += amount; }
    void setScrollWheel(s32 value)  { mouse.scrollWheel = value; }
    s32  scrollWheel() const        { return mouse.scrollWheel; }
    s32  scrollDelta() const        { return mouse.scrollWheel - previousMouse.scrollWheel; }

    void setGamepadConnected(s32 index, b32 connected) { pad(index).connected = connected; }
    b32  isGamepadConnected(s32 index) const           { return pad(index).connected; }

    b32 isGamepadButtonDown(s32 index, GamepadButton button) const
    {
        return pad(index).buttons[toIndex(button)] == InputState::Down;
    }

    b32 isGamepadButtonUp(s32 index, GamepadButton button) const
    {
        return pad(index).buttons[toIndex(button)] == InputState::Up;
    }

    b32 isGamepadButtonPressed(s32 index, GamepadButton button) const
    {
        return wasPressed(pad(index).buttons[toIndex(button)],
                          previousPad(index).buttons[toIndex(button)]);
    }

    b32 isGamepadButtonReleased(s32 index, GamepadButton button) const
    {
        return wasReleased(pad(index).buttons[toIndex(button)],
                           previousPad(index).buttons[toIndex(button)]);
    }

    void setGamepadButton(s32 index, GamepadButton button, InputState state)
    {
        pad(index).buttons[toIndex(button)] = state;
    }

    void setLeftStick(s32 index, f32 x, f32 y)  { pad(index).leftStickX = x;  pad(index).leftStickY = y; }
    void setRightStick(s32 index, f32 x, f32 y) { pad(index).rightStickX = x; pad(index).rightStickY = y; }
    void setLeftTrigger(s32 index, f32 value)   { pad(index).leftTrigger = value; }
    void setRightTrigger(s32 index, f32 value)  { pad(index).rightTrigger = value; }

    vec2 leftStick(s32 index) const   { return vec2{pad(index).leftStickX, pad(index).leftStickY}; }
    vec2 rightStick(s32 index) const  { return vec2{pad(index).rightStickX, pad(index).rightStickY}; }
    f32  leftTrigger(s32 index) const { return pad(index).leftTrigger; }
    f32  rightTrigger(s32 index) const{ return pad(index).rightTrigger; }

private:
    template <typename E>
    static constexpr s32 toIndex(E value) { return static_cast<s32>(value); }

    static b32 wasPressed(InputState current, InputState previous)
    {
        return current == InputState::Down && previous == InputState::Up;
    }

    static b32 wasReleased(InputState current, InputState previous)
    {
        return current == InputState::Up && previous == InputState::Down;
    }

    Gamepad& pad(s32 index)
    {
        assert(index >= 0 && index < MaxGamepads);
        return gamepads[index];
    }

    const Gamepad& pad(s32 index) const
    {
        assert(index >= 0 && index < MaxGamepads);
        return gamepads[index];
    }

    const Gamepad& previousPad(s32 index) const
    {
        assert(index >= 0 && index < MaxGamepads);
        return previousGamepads[index];
    }

};
