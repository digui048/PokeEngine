#include "EditorModule.h"

#include "Poke/Core/Time.h"

#include "Poke/Render/Renderer.h"

#include "PokeEngineEditor/Interfaces/HierarchyInterface.h"
#include "PokeEngineEditor/Interfaces/MainMenuBarInterface.h"
#include "PokeEngineEditor/Interfaces/InspectorInterface.h"
#include "PokeEngineEditor/Interfaces/AssetsExplorerInterface.h"

#include "PokeEngineEditor/Asset/EditorAssetManager.h"

#include "Poke/Resources/AssetManager.h"
#include "Poke/Resources/Assets/Model.h"

#include "Poke/Core/Application.h"
#include "Poke/Core/Window.h"

#include "Poke/Render/UniformBuffer.h"
#include "Poke/Render/Vulkan/VulkanPipeline.h"
#include "Poke/Render/Vulkan/VulkanTexture.h"
#include "Poke/Importers/MeshImporter.h"
#include "Poke/Importers/TextureImporter.h"
#include "Poke/Importers/MaterialImporter.h"

#include "Poke/Scene/EditorCamera.h"
#include "Poke/Scene/Scene.h"
#include "Poke/Scene/Components/MeshRendererComponent.h"
#include "Poke/Scene/Components/TransformComponent.h"

using namespace Poke;

EditorModule::EditorModule(const std::string &projectName, const std::filesystem::path &projectDir)
    : m_projectName(projectName), m_projectDir(projectDir)
{
}

EditorModule::~EditorModule() = default;

void EditorModule::OnInit()
{
    m_assetManager = std::make_unique<EditorAssetManager>(m_projectDir);
    AssetManager::SetActive(m_assetManager.get());
    m_assetManager->ScanDirectoryAssets(m_projectDir / "Assets");

    m_scene = std::make_unique<Scene>();
    m_scene->OnInit();

    AddInterface<HierarchyInterface>(m_scene.get());
    AddInterface<InspectorInterface>();
    AddInterface<MainMenuBarInterface>();
    AddInterface<AssetsExplorerInterface>(m_projectDir, m_assetManager.get());

    AssetHandle shibaHandle = m_assetManager->GetAssetHandle(m_projectDir / "Assets/shiba.fbx");
    m_shibaEntity = m_scene->InstantiateModel(shibaHandle, "Shiba");

    VkPushConstantRange pushConstantRange;
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(ObjectData);
    std::vector<VkPushConstantRange> pushConstantRanges{pushConstantRange};

    m_defaultPipeline = Renderer::CreatePipeline("Poke/assets/shaders/defaultShader.vert.spv", "Poke/assets/shaders/defaultShader.frag.spv", pushConstantRanges);
    m_defaultPipeline->SetupGlobalDescriptors(Renderer::GetDefaultUniformBuffer());

    // m_textureS = TextureImporter::LoadTexture((m_projectDir / "Assets/default_Base_Color.png").string());
    std::filesystem::path texturePath = m_projectDir / "Assets/default_Base_Color.png";
    AssetHandle textureHandle = m_assetManager->GetAssetHandle(texturePath);
    if (AssetManager::GetAssetType(textureHandle) == AssetType::Texture)
    {
        m_textureS = AssetManager::GetAsset<Texture>(textureHandle);
    }

    m_materialS = MaterialImporter::LoadMaterial(m_textureS, m_defaultPipeline.get());

    for (auto &child : m_shibaEntity->GetChildren())
    {
        if (auto *mesh = child->GetComponent<MeshRendererComponent>())
        {
            mesh->SetMaterial(m_materialS);
        }
    }

    int w, h;
    Application::GetInstance().GetWindow()->GetWindowSize(w, h);
    EditorCamera::Get().Init(w, h);
}

void EditorModule::OnUpdate(float dt)
{
    for (auto &interface : m_Interfaces)
    {
        if (interface->IsOpen())
        {
            interface->OnUpdate(dt);
        }
    }

    m_scene->OnUpdate(dt);

    EditorCamera::Get().OnUpdate(dt);

    int w, h;
    Application::GetInstance().GetWindow()->GetWindowSize(w, h);
    EditorCamera::Get().Resize(w, h);

    Renderer::UpdateCameraBuffer(EditorCamera::Get().GetViewMatrix(), EditorCamera::Get().GetProjectionMatrix());
}

void EditorModule::OnRender(VkCommandBuffer cmd)
{
    RenderWorld(cmd);
}

void EditorModule::OnImGuiRender()
{
    for (auto &interface : m_Interfaces)
    {
        interface->OnImGuiRender();
    }
}

void EditorModule::OnShutdown()
{
    for (auto &interface : m_Interfaces)
    {
        interface->OnShutdown();
    }
    m_Interfaces.clear();
    m_scene->OnShutdown();
    m_textureS.reset();
    m_materialS.reset();
    m_defaultPipeline.reset();

    AssetManager::Shutdown();
    m_assetManager.reset();
}

void EditorModule::OnFileDropped(const char *path, float x, float y)
{
    auto *assetInterface = GetInterface<AssetsExplorerInterface>();
    if (assetInterface)
    {
        assetInterface->OnFileDropped(path, x, y);
    }
}

void EditorModule::RenderWorld(VkCommandBuffer cmd)
{
    if (!m_scene || !m_scene->GetRoot())
        return;

    std::vector<GameObject *> entities;
    std::vector<GameObject *> traversalStack = {m_scene->GetRoot()};

    while (!traversalStack.empty())
    {
        GameObject *entity = traversalStack.back();
        traversalStack.pop_back();

        if (!entity || !entity->IsActive())
            continue;

        entities.push_back(entity);

        for (const auto &child : entity->GetChildren())
        {
            traversalStack.push_back(child.get());
        }
    }

    for (GameObject *entity : entities)
    {
        auto *meshComp = entity->GetComponent<MeshRendererComponent>();
        if (!meshComp)
            continue;

        auto mesh = meshComp->GetMesh();
        if (!mesh)
            continue;

        auto material = meshComp->GetMaterial();

        Renderer::SubmitRenderItem(mesh, material, entity->GetTransform()->GetWorldTransform());
    }

    Renderer::FlushQueue(cmd, m_defaultPipeline);
}
