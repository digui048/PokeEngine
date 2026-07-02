#include "MainMenuBarInterface.h"

#include <imgui.h>

using namespace Poke;

void MainMenuBarInterface::OnInit()
{
}

void MainMenuBarInterface::OnImGuiRender()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit"))
            {
                // Quit
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}