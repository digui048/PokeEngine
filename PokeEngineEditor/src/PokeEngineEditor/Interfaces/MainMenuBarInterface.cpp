#include "MainMenuBarInterface.h"
#include "Poke/Core/Application.h"

#include "PokeEngineEditor/Interfaces/HierarchyInterface.h"

#include "PokeEngineEditor/Asset/EditorAssetManager.h"
#include "Poke/Resources/AssetManager.h"
#include "Poke/Scene/Scene.h"
#include "Poke/Core/Log.h"

#include <imgui.h>
#include <memory>

using namespace Poke;

MainMenuBarInterface::MainMenuBarInterface(EditorAssetManager *assetManager, const std::filesystem::path &projectDir)
    : EditorInterface("MainMenuBar"), m_assetManager(assetManager), m_projectDir(projectDir)
{
}

void MainMenuBarInterface::OnInit()
{
}

void MainMenuBarInterface::OnImGuiRender()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
            {
                std::shared_ptr<Scene> activeScene = HierarchyInterface::GetActiveScene();
                if (activeScene)
                {
                    const std::filesystem::path scenePath = m_projectDir / "Assets/MainScene.scene";
                    auto sharedScene = AssetManager::GetAsset<Scene>(activeScene->GetHandle());

                    m_assetManager->SaveScene(sharedScene, scenePath);
                    POKE_CORE_INFO("[MainMenuBar] Scene saved successfully to: {0}", scenePath.string());
                }
                else
                    POKE_CORE_ERROR("[MainMenuBar] No active scene to save");
            }

            if (ImGui::MenuItem("Load Scene", "Ctrl+O"))
            {
                const std::filesystem::path scenePath = m_projectDir / "Assets/MainScene.scene";
                std::shared_ptr<Scene> loadedScene = m_assetManager->LoadScene(scenePath);
                if (loadedScene)
                {
                    HierarchyInterface::SetActiveScene(loadedScene);
                    POKE_CORE_INFO("[MainMenuBar] Scene loaded successfully from: {0}", scenePath.string());
                }
                else
                {
                    POKE_CORE_ERROR("[MainMenuBar] Failed to load scene from: {0}", scenePath.string());
                }
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Exit"))
            {
                Application::GetInstance().ForceQuit();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}