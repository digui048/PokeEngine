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

    ImGui_ImplSDL3_InitForOpenGL(Application::GetInstance().GetInstance().GetWindow()->GetSDLWindow(),
                                 Application::GetInstance().GetInstance().GetWindow()->GetSDLContext());
    ImGui_ImplOpenGL3_Init();

    CustomImGui();
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

void Poke::ImGuiManager::CustomImGui()
{
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.IniFilename = "PokeEngineEditor/assets/imgui.ini";
}
