#include "Input.h"

using namespace Poke;

SDL_Window *Input::s_windowHandle = nullptr;
std::vector<KeyState> Input::s_keyboardState{};
KeyState Input::s_mouseState[6] = {KEY_IDLE};

glm::vec2 Input::s_currentMousePos = {0.0f, 0.0f};
glm::vec2 Input::s_mouseDelta = {0.0f, 0.0f};
bool Input::s_ignoreNextRelativeDelta = false;
bool Input::s_relativeMouseMode = false;

void Input::Init(SDL_Window *handle)
{
    s_windowHandle = handle;
    s_keyboardState.resize(SDL_SCANCODE_COUNT, KEY_IDLE);
}

void Input::Update()
{
    int numKeys = 0;
    const bool *sdlKeyStates = SDL_GetKeyboardState(&numKeys);

    for (int i = 0; i < numKeys && i < SDL_SCANCODE_COUNT; ++i)
    {
        bool isPressed = sdlKeyStates[i];
        KeyState &currentState = s_keyboardState[i];

        if (isPressed)
        {
            if (currentState == KEY_IDLE || currentState == KEY_UP)
            {
                currentState = KEY_DOWN;
            }
            else if (currentState == KEY_DOWN)
            {
                currentState = KEY_REPEAT;
            }
        }
        else
        {
            if (currentState == KEY_DOWN || currentState == KEY_REPEAT)
            {
                currentState = KEY_UP;
            }
            else if (currentState == KEY_UP)
            {
                currentState = KEY_IDLE;
            }
        }
    }

    float posX;
    float posY;
    Uint32 mouseFlags = SDL_GetMouseState(&posX, &posY);
    s_currentMousePos = {posX, posY};

    for (Uint8 button = 1; button <= 5; ++button)
    {
        bool isPressed = (mouseFlags & SDL_BUTTON_MASK(button)) != 0;
        KeyState &currentState = s_mouseState[button];

        if (isPressed)
        {
            if (currentState == KEY_IDLE || currentState == KEY_UP)
            {
                currentState = KEY_DOWN;
            }
            else if (currentState == KEY_DOWN)
            {
                currentState = KEY_REPEAT;
            }
        }
        else
        {
            if (currentState == KEY_DOWN || currentState == KEY_REPEAT)
            {
                currentState = KEY_UP;
            }
            else if (currentState == KEY_UP)
            {
                currentState = KEY_IDLE;
            }
        }
    }
    if (s_relativeMouseMode)
    {
        float deltaX;
        float deltaY;

        SDL_GetRelativeMouseState(&deltaX, &deltaY);

        if (s_ignoreNextRelativeDelta)
        {
            s_mouseDelta = {0.0f, 0.0f};
            s_ignoreNextRelativeDelta = false;
        }
        else
        {
            s_mouseDelta = {deltaX, deltaY};
        }
    }
    else
    {
        s_mouseDelta = {0.0f, 0.0f};
    }
}

KeyState Input::GetKeyState(SDL_Scancode scancode)
{
    return s_keyboardState[scancode];
}

bool Input::IsKeyDown(SDL_Scancode scancode)
{
    return s_keyboardState[scancode] == KEY_DOWN;
}

bool Input::IsKeyHeld(SDL_Scancode scancode)
{
    return s_keyboardState[scancode] == KEY_DOWN || s_keyboardState[scancode] == KEY_REPEAT;
}

bool Input::IsKeyUp(SDL_Scancode scancode)
{
    return s_keyboardState[scancode] == KEY_UP;
}

KeyState Input::GetMouseButtonState(Uint8 button)
{
    if (button >= 1 && button <= 5)
    {
        return s_mouseState[button];
    }
    return KEY_IDLE;
}

bool Input::IsMouseButtonDown(Uint8 button)
{
    return GetMouseButtonState(button) == KEY_DOWN;
}

bool Input::IsMouseButtonHeld(Uint8 button)
{
    KeyState state = GetMouseButtonState(button);
    return state == KEY_DOWN || state == KEY_REPEAT;
}

bool Input::IsMouseButtonUp(Uint8 button)
{
    return GetMouseButtonState(button) == KEY_UP;
}

glm::vec2 Input::GetMousePosition()
{
    return s_currentMousePos;
}

glm::vec2 Input::GetMouseDelta()
{
    return s_mouseDelta;
}

void Input::SetCursorMode(bool relativeMode)
{
    if (s_relativeMouseMode == relativeMode)
    {
        return;
    }

    SDL_SetWindowRelativeMouseMode(s_windowHandle, relativeMode);
    s_relativeMouseMode = relativeMode;

    if (relativeMode)
    {
        s_ignoreNextRelativeDelta = true;
    }
    else
    {
        s_mouseDelta = {0.0f, 0.0f};
    }
}
