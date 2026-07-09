#include "ImGuiManager.h"
#include "Window.h"
#include "Application.h"

using namespace Poke;

ImGuiManager::~ImGuiManager()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiManager::Init()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplSDL3_InitForVulkan(Application::GetInstance().GetInstance().GetWindow()->GetSDLWindow());
    //ImGui_ImplVulkan_Init()
}

void ImGuiManager::BeginFrame()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::EndFrame()
{
    ImGui::Render();
    //ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData());
}

void Poke::ImGuiManager::CustomImGui()
{
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.IniFilename = "PokeEngineEditor/assets/imgui.ini";
}
