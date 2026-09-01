#ifndef INPUT_H
#define INPUT_H

#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <vector>

namespace Poke
{
    enum KeyState
    {
        KEY_IDLE = 0,
        KEY_DOWN,
        KEY_REPEAT,
        KEY_UP
    };

    class Input
    {
    public:
        Input() = delete;

        static void Init(SDL_Window *handle);
        static void Update();

        static KeyState GetKeyState(SDL_Scancode scancode);
        static bool IsKeyDown(SDL_Scancode scancode);
        static bool IsKeyHeld(SDL_Scancode scancode);
        static bool IsKeyUp(SDL_Scancode scancode);

        static KeyState GetMouseButtonState(Uint8 button);
        static bool IsMouseButtonDown(Uint8 button);
        static bool IsMouseButtonHeld(Uint8 button);
        static bool IsMouseButtonUp(Uint8 button);

        static glm::vec2 GetMousePosition();
        static glm::vec2 GetMouseDelta();
        static void SetCursorMode(bool relativeMode);

    private:
        static SDL_Window *s_windowHandle;

        static std::vector<KeyState> s_keyboardState;
        static KeyState s_mouseState[6];

        static glm::vec2 s_currentMousePos;
        static glm::vec2 s_mouseDelta;
        static bool s_relativeMouseMode;
        static bool s_ignoreNextRelativeDelta;
    };
}

#endif