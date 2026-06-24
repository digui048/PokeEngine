#ifndef IMGUI_MANAGER_H
#define IMGUI_MANAGER_H

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

namespace Poke
{
    class ImGuiManager
    {
    public:
        ImGuiManager() = default;
        ~ImGuiManager();

        void Init();
        void GetEvents();

        void BeginFrame();
        void EndFrame();

    private:
        void CustomImGui();
    };
}

#endif