#pragma once

#include "Module.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include "Vector2D.h"

#define NUM_MOUSE_BUTTONS 5

enum EventWindow
{
    WE_QUIT = 0,
    WE_HIDE = 1,
    WE_SHOW = 2,
    WE_COUNT
};

enum KeyState
{
    KEY_IDLE = 0,
    KEY_DOWN,
    KEY_REPEAT,
    KEY_UP
};

class Input : public Module
{
public:

    Input();
    virtual ~Input();

    bool Awake();
    bool Start();
    bool PreUpdate();
    bool CleanUp();

    KeyState GetKey(int id) const { return keyboard[id]; }
    KeyState GetMouseButtonDown(int id) const { return mouseButtons[id - 1]; }

    bool GetWindowEvent(EventWindow ev);

    Vector2D GetMousePosition();
    Vector2D GetMouseMotion();

    SDL_Gamepad* GetGamepad() { return gamepad; }

    // --- GAMEPAD GETTERS ---
    float GetAxisLeftX() const { return axisLX; }
    float GetAxisLeftY() const { return axisLY; }
    float GetAxisRightX() const { return axisRX; }
    float GetAxisRightY() const { return axisRY; }
    bool GetButtonSouth() const { return buttonSouth; }

    bool GetButton(SDL_GamepadButton button) const {
        return SDL_GetGamepadButton(gamepad, button);
    }

private:

    bool windowEvents[WE_COUNT];
    KeyState* keyboard;
    KeyState mouseButtons[NUM_MOUSE_BUTTONS];

    int mouseMotionX;
    int mouseMotionY;
    int mouseX;
    int mouseY;

    SDL_Gamepad* gamepad = nullptr;

    // --- GAMEPAD STATE ---
    float axisLX = 0.0f;
    float axisLY = 0.0f;
    float axisRX = 0.0f;
    float axisRY = 0.0f;
    bool buttonSouth = false;
    bool lastButtonSouth = false;

};