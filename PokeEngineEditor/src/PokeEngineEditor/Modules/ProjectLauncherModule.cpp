#include "ProjectLauncherModule.h"
#include "Poke/Core/Log.h"
#include "Poke/Core/Window.h"
#include "Poke/Core/Application.h"
#include "PokeEngineEditor/Modules/EditorModule.h"

#include <SDL3/SDL.h>
#include <filesystem>
#include <imgui.h>

using namespace Poke;

static const SDL_DialogFileFilter filters[] = {
    {"Poke Engine", "poke"},
    {"All Files", "*"}};

static void SDLCALL OnCreateProjectCallback(void *userdata, const char *const *fileList, int filter)
{
    if (!fileList || !*fileList)
    {
        POKE_CORE_WARN("Dialog canceled or error occurred: {0}", SDL_GetError());
        return;
    }

    auto *launcher = static_cast<ProjectLauncherModule *>(userdata);
    launcher->CreateNewProject(fileList[0]);
}

static void SDLCALL OnOpenProjectCallback(void *userdata, const char *const *fileList, int filter)
{
    if (!fileList || !*fileList)
    {
        POKE_CORE_WARN("Dialog canceled or error occurred: {0}", SDL_GetError());
        return;
    }

    auto *launcher = static_cast<ProjectLauncherModule *>(userdata);
    launcher->OpenExistingProject(fileList[0]);
}

ProjectLauncherModule::ProjectLauncherModule() = default;
ProjectLauncherModule::~ProjectLauncherModule() = default;

void ProjectLauncherModule::OnImGuiRender()
{
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

    if (ImGui::Begin("Project Launcher", nullptr, flags))
    {
        auto windowWidth = ImGui::GetWindowSize().x;
        auto textWidth = ImGui::CalcTextSize("POKE ENGINE").x;
        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::Text("POKE ENGINE");

        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 30.0f));

        SDL_Window* window = Application::GetInstance().GetWindow()->GetSDLWindow();

        if (ImGui::Button("Create New Project", ImVec2(-1.0f, 50.0f)))
        {
            const char* defaultFolder = SDL_GetUserFolder(SDL_FOLDER_DESKTOP);
            SDL_ShowSaveFileDialog(OnCreateProjectCallback, this, window, filters, 2, defaultFolder);
        }

        ImGui::Dummy(ImVec2(0.0f, 15.0f));

        if(ImGui::Button("Open Existing Project", ImVec2(-1.0f, 50.0f)))
        {
            const char* defaultFolder = SDL_GetUserFolder(SDL_FOLDER_DESKTOP);
            SDL_ShowOpenFileDialog(OnOpenProjectCallback, this, window, filters, 2, defaultFolder, false);
        }

        ImGui::End();
    }
}

void ProjectLauncherModule::CreateNewProject(const std::string &path)
{
    std::filesystem::path projectFilePath = path;

    if (projectFilePath.extension() != ".poke")
    {
        projectFilePath += ".poke";
    }

    std::filesystem::path projectDir = projectFilePath.parent_path();
    std::string projectName = projectFilePath.stem().string();

    if (!std::filesystem::exists(projectDir))
    {
        std::filesystem::create_directories(projectDir);
        POKE_CORE_INFO("Created new project folder: {0}", projectDir.string());
    }

    SDL_IOStream *file = SDL_IOFromFile(projectFilePath.string().c_str(), "w");
    if (file)
    {
        std::string defaultContent = "{\n \"projectName\": \"" + projectName + "\",\n \"version\": \"1.0\"\n}";
        SDL_WriteIO(file, defaultContent.c_str(), defaultContent.size());
        SDL_CloseIO(file);

        POKE_CORE_INFO("Created project file: {0}", projectFilePath.string());
    }
    else
    {
        POKE_CORE_ERROR("Failed to create project file, SDL Error: {0}", SDL_GetError());
        return;
    }

    Application::GetInstance().SwitchModule(std::make_shared<EditorModule>(projectName, projectDir));
}

void ProjectLauncherModule::OpenExistingProject(const std::string &path)
{
    std::filesystem::path projectFilePath = path;

    std::filesystem::path projectDir = projectFilePath.parent_path();
    std::string projectName = projectFilePath.stem().string();

    POKE_CORE_INFO("Opening project: {0} at {1}", projectName, projectDir.string());

    Application::GetInstance().SwitchModule(std::make_shared<EditorModule>(projectName, projectDir));
}
