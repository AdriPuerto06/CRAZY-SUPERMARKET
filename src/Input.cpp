#include "Engine.h"
#include "Input.h"
#include "Window.h"
#include "Log.h"

#define MAX_KEYS 300

Input::Input() : Module()
{
    name = "input";

    keyboard = new KeyState[MAX_KEYS];
    memset(keyboard, KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
    memset(mouseButtons, KEY_IDLE, sizeof(KeyState) * NUM_MOUSE_BUTTONS);
    memset(windowEvents, 0, sizeof(windowEvents));

    mouseMotionX = mouseMotionY = mouseX = mouseY = 0;
}

Input::~Input()
{
    delete[] keyboard;
}

bool Input::Awake()
{
    LOG("Init SDL input event system");

    if (!SDL_InitSubSystem(SDL_INIT_EVENTS))
    {
        LOG("SDL_EVENTS could not initialize! SDL_Error: %s", SDL_GetError());
        return false;
    }

    return true;
}

bool Input::Start()
{
    // --- GAMEPAD INIT (SDL3) ---
    int count = 0;
    SDL_JoystickID* pads = SDL_GetGamepads(&count);

    if (count > 0)
    {
        gamepad = SDL_OpenGamepad(pads[0]);
        if (gamepad)
            LOG("Gamepad conectado correctamente");
    }

    SDL_free(pads);

    SDL_StopTextInput(Engine::GetInstance().window->window);
    return true;
}

bool Input::PreUpdate()
{
    static SDL_Event event;

    // --- TECLADO ---
    int numKeys = 0;
    const bool* keys = SDL_GetKeyboardState(&numKeys);

    for (int i = 0; i < MAX_KEYS; ++i)
    {
        if (keys[i])
        {
            keyboard[i] = (keyboard[i] == KEY_IDLE) ? KEY_DOWN : KEY_REPEAT;
        }
        else
        {
            keyboard[i] = (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN) ? KEY_UP : KEY_IDLE;
        }
    }

    // --- RATÓN ---
    for (int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
    {
        if (mouseButtons[i] == KEY_DOWN)
            mouseButtons[i] = KEY_REPEAT;
        if (mouseButtons[i] == KEY_UP)
            mouseButtons[i] = KEY_IDLE;
    }

    // --- EVENTOS SDL ---
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            windowEvents[WE_QUIT] = true;
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (event.button.button >= 1 && event.button.button <= NUM_MOUSE_BUTTONS)
                mouseButtons[event.button.button - 1] = KEY_DOWN;
            break;

        case SDL_EVENT_MOUSE_BUTTON_UP:
            if (event.button.button >= 1 && event.button.button <= NUM_MOUSE_BUTTONS)
                mouseButtons[event.button.button - 1] = KEY_UP;
            break;

        case SDL_EVENT_MOUSE_MOTION:
        {
            float scale = Engine::GetInstance().window->GetScale();

            mouseX = (int)(event.motion.x / scale);
            mouseY = (int)(event.motion.y / scale);

            mouseMotionX = (int)(event.motion.xrel / scale);
            mouseMotionY = (int)(event.motion.yrel / scale);
        }
        break;
        }
    }

    // --- 1) LEER GAMEPAD ---
    if (gamepad)
    {
        auto norm = [](Sint16 v) { return (float)v / 32767.0f; };

        axisLX = norm(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX));
        axisLY = norm(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY));
        axisRX = norm(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTX));
        axisRY = norm(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTY));

        buttonSouth = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_SOUTH);
    }

    // --- 2) MOVER RATÓN CON STICK DERECHO ---
    {
        const float deadzone = 0.2f;
        const float speed = 12.0f;

        float rx = (fabs(axisRX) > deadzone) ? axisRX : 0;
        float ry = (fabs(axisRY) > deadzone) ? axisRY : 0;

        if (rx != 0 || ry != 0)
        {
            float mx, my;
            SDL_GetMouseState(&mx, &my);

            mx += rx * speed;
            my += ry * speed;

            SDL_WarpMouseInWindow(Engine::GetInstance().window->window, (int)mx, (int)my);
        }
    }

    // --- 3) SIMULAR CLICK IZQUIERDO CON BOTÓN A/X ---
    {
        SDL_Event e;

        if (buttonSouth)
            e.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
        else
            e.type = SDL_EVENT_MOUSE_BUTTON_UP;

        e.button.button = SDL_BUTTON_LEFT;
        e.button.clicks = 1;

        SDL_GetMouseState(&e.button.x, &e.button.y);
        SDL_PushEvent(&e);
    }

    return true;
}

bool Input::CleanUp()
{
    LOG("Quitting SDL event subsystem");
    SDL_QuitSubSystem(SDL_INIT_EVENTS);
    return true;
}

bool Input::GetWindowEvent(EventWindow ev)
{
    return windowEvents[ev];
}

Vector2D Input::GetMousePosition()
{
    return Vector2D((float)mouseX, (float)mouseY);
}

Vector2D Input::GetMouseMotion()
{
    return Vector2D((float)mouseMotionX, (float)mouseMotionY);
}
