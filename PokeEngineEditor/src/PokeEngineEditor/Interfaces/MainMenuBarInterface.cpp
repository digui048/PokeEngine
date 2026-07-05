#include "MainMenuBarInterface.h"
#include "Poke/Core/Application.h"

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
                Application::GetInstance().ForceQuit();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}