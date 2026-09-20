#include "ProjectLauncherModule.h"
#include "Poke/Core/Log.h"
#include "Poke/Core/Window.h"
#include "Poke/Core/Application.h"
#include "PokeEngineEditor/Modules/EditorModule.h"

#include <SDL3/SDL.h>
#include <filesystem>
#include <imgui.h>
#include <fstream>

using namespace Poke;

static const SDL_DialogFileFilter filters[] = {
    {"Poke Engine", "poke"},
    {"All Files", "*"}};

static std::string GetConfigFilePath()
{
    std::filesystem::path configDir = "PokeEngineEditor/config";
    return (configDir / "recent_projects.txt").string();
}

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

void ProjectLauncherModule::OnInit()
{
    LoadRecentProjects();
}

void ProjectLauncherModule::OnImGuiRender()
{
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    if (ImGui::Begin("PokeHubLayout", nullptr, flags))
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.11f, 0.11f, 0.12f, 1.0f));
        ImGui::BeginChild("Sidebar", ImVec2(220.0f, 0.0f), false);

        ImGui::Dummy(ImVec2(0.0f, 20.0f));
        ImGui::SetCursorPosX(20.0f);
        ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1.0f), "POKE HUB");
        ImGui::Dummy(ImVec2(0.0f, 20.0f));

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 8.0f));
        ImGui::Selectable("   Projects", true, 0, ImVec2(220.0f, 35.0f));
        ImGui::PopStyleVar();

        ImGui::EndChild();
        ImGui::PopStyleColor();

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.16f, 0.16f, 0.17f, 1.0f));
        ImGui::BeginChild("MainArea", ImVec2(0.0f, 0.0f), false);

        ImGui::Dummy(ImVec2(0.0f, 20.0f));
        ImGui::SetCursorPosX(30.0f);

        ImGui::BeginGroup();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Projects");

        SDL_Window *window = Application::GetInstance().GetWindow()->GetSDLWindow();
        const char *defaultFolder = SDL_GetUserFolder(SDL_FOLDER_DESKTOP);

        float contentWidth = ImGui::GetWindowWidth();
        ImGui::SameLine(contentWidth - 250.0f);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.24f, 0.24f, 0.26f, 1.0f));
        if (ImGui::Button("ADD", ImVec2(100.0f, 36.0f)))
        {
            SDL_ShowOpenFileDialog(OnOpenProjectCallback, this, window, filters, 2, defaultFolder, false);
        }
        ImGui::PopStyleColor();

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.47f, 0.84f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.55f, 0.95f, 1.0f));
        if (ImGui::Button("NEW", ImVec2(100.0f, 36.0f)))
        {
            SDL_ShowSaveFileDialog(OnCreateProjectCallback, this, window, filters, 2, defaultFolder);
        }
        ImGui::PopStyleColor(2);

        ImGui::EndGroup();

        ImGui::Dummy(ImVec2(0.0f, 20.0f));
        ImGui::SetCursorPosX(30.0f);

        ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg |
                                     ImGuiTableFlags_PadOuterX |
                                     ImGuiTableFlags_SizingStretchSame;

        if (ImGui::BeginTable("ProjectsTable", 3, tableFlags, ImVec2(contentWidth - 60.0f, 0.0f)))
        {
            ImGui::TableSetupColumn("Project Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Engine Version", ImGuiTableColumnFlags_WidthFixed, 140.0f);
            ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 110.0f);
            ImGui::TableHeadersRow();

            int projectToRemove = -1;

            for (size_t i = 0; i < m_recentProjects.size(); i++)
            {
                ImGui::PushID(static_cast<int>(i));

                const auto &project = m_recentProjects[i];
                bool exists = std::filesystem::exists(project.path);

                ImGui::TableNextRow(0, 45.0f);
                ImGui::TableSetColumnIndex(0);

                if (ImGui::BeginPopupContextItem("ProjectRowContext"))
                {
                    if (ImGui::MenuItem("Remove from list"))
                    {
                        projectToRemove = static_cast<int>(i);
                    }
                    ImGui::EndPopup();
                }

                ImGuiSelectableFlags selectableFlags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap;
                ImGui::Selectable(project.name.c_str(), false, selectableFlags, ImVec2(0.0f, 35.0f));

                if (exists)
                {
                    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "%s", project.path.c_str());
                }
                else
                {
                    ImGui::TextColored(ImVec4(0.85f, 0.3f, 0.3f, 1.0f), "%s (File missing)", project.path.c_str());
                }

                ImGui::TableSetColumnIndex(1);
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
                ImGui::TextUnformatted("v1.0.0");

                ImGui::TableSetColumnIndex(2);
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

                ImGui::BeginDisabled(!exists);
                if (ImGui::Button("Open", ImVec2(60.0f, 25.0f)))
                {
                    OpenExistingProject(project.path);
                }
                ImGui::EndDisabled();

                ImGui::SameLine();

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.15f, 0.15f, 0.6f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));

                std::string btnId = "X##" + std::to_string(i);
                if (ImGui::Button(btnId.c_str(), ImVec2(25.0f, 25.0f)))
                {
                    projectToRemove = static_cast<int>(i);
                }
                ImGui::PopStyleColor(3);

                ImGui::PopID();
            }

            ImGui::EndTable();

            if (projectToRemove != -1)
            {
                RemoveFromRecentProjects(projectToRemove);
            }
        }

        ImGui::EndChild();
        ImGui::PopStyleColor();

        ImGui::End();
    }

    ImGui::PopStyleVar(2);
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

    AddToRecentProjects(projectName, projectFilePath.string());
    Application::GetInstance().SwitchModule(std::make_shared<EditorModule>(projectName, projectDir));
}

void ProjectLauncherModule::OpenExistingProject(const std::string &path)
{
    std::filesystem::path projectFilePath = path;

    std::filesystem::path projectDir = projectFilePath.parent_path();
    std::string projectName = projectFilePath.stem().string();

    POKE_CORE_INFO("Opening project: {0} at {1}", projectName, projectDir.string());

    AddToRecentProjects(projectName, projectFilePath.string());
    Application::GetInstance().SwitchModule(std::make_shared<EditorModule>(projectName, projectDir));
}

void ProjectLauncherModule::LoadRecentProjects()
{
    m_recentProjects.clear();
    std::ifstream file(GetConfigFilePath());
    if (!file.is_open())
        return;

    std::string name;
    std::string path;
    while (std::getline(file, name) && std::getline(file, path))
    {
        m_recentProjects.push_back({name, path});
    }
}

void ProjectLauncherModule::SaveRecentProjects()
{
    std::ofstream file(GetConfigFilePath());
    if (!file.is_open())
        return;

    for (const auto &proj : m_recentProjects)
    {
        file << proj.name << "\n"
             << proj.path << "\n";
    }
}

void ProjectLauncherModule::AddToRecentProjects(const std::string &name, const std::string &path)
{
    for (auto it = m_recentProjects.begin(); it != m_recentProjects.end(); ++it)
    {
        if (it->path == path)
        {
            m_recentProjects.erase(it);
            break;
        }
    }

    m_recentProjects.insert(m_recentProjects.begin(), {name, path});
    SaveRecentProjects();
}

void ProjectLauncherModule::RemoveFromRecentProjects(size_t index)
{
    if (index < m_recentProjects.size())
    {
        m_recentProjects.erase(m_recentProjects.begin() + index);
        SaveRecentProjects();
    }
}
