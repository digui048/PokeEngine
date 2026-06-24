#include "ImGuiManager.h"
#include "Window.h"
#include "Application.h"

using namespace Poke;

ImGuiManager::~ImGuiManager()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiManager::Init()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplSDL3_InitForOpenGL(Application::GetInstance().GetInstance().GetWindow()->GetSDLWindow(),
                                 Application::GetInstance().GetInstance().GetWindow()->GetSDLContext());
    ImGui_ImplOpenGL3_Init();
}

void ImGuiManager::BeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::EndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
