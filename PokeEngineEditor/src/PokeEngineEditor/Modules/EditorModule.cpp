#include "EditorModule.h"

#include "Poke/Core/Time.h"

#include "Poke/Render/Renderer.h"

#include "PokeEngineEditor/Interfaces/HierarchyInterface.h"
#include "PokeEngineEditor/Interfaces/MainMenuBarInterface.h"
#include "PokeEngineEditor/Interfaces/InspectorInterface.h"

#include "Poke/Core/Application.h"
#include "Poke/Core/Window.h"

#include "Poke/Render/UniformBuffer.h"
#include "Poke/Render/Vulkan/VulkanPipeline.h"
#include "Poke/Render/Vulkan/VulkanTexture.h"
#include "Poke/Importers/MeshImporter.h"
#include "Poke/Importers/TextureImporter.h"

#include "Poke/Scene/EditorCamera.h"
#include "Poke/Scene/Scene.h"
#include "Poke/Scene/Components/MeshComponent.h"
#include "Poke/Scene/Components/TransformComponent.h"
#include "Poke/Resources/Mesh.h"

using namespace Poke;

EditorModule::EditorModule() = default;
EditorModule::~EditorModule() = default;

void EditorModule::OnInit()
{
    m_scene = std::make_unique<Scene>();
    m_scene->OnInit();

    AddInterface<HierarchyInterface>(m_scene.get());
    AddInterface<InspectorInterface>();
    AddInterface<MainMenuBarInterface>();

    m_shibaEntity = m_scene->CreateGameObject("Shiba");
    MeshImporter::LoadHierarchy("Poke/assets/shiba.fbx", m_shibaEntity);

    VkPushConstantRange pushConstantRange;
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(ObjectData);
    std::vector<VkPushConstantRange> pushConstantRanges{pushConstantRange};

    m_defaultPipeline = Renderer::CreatePipeline("Poke/assets/shaders/defaultShader.vert.spv", "Poke/assets/shaders/defaultShader.frag.spv", pushConstantRanges);

    m_texture = TextureImporter::LoadTexture("Poke/assets/default_Base_Color.png");

    m_defaultPipeline->SetupDescriptors(Renderer::GetDefaultUniformBuffer(), m_texture->GetVulkanTexture());

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
    m_texture.reset();
    m_defaultPipeline.reset();
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
        auto *meshComp = entity->GetComponent<MeshComponent>();
        if (!meshComp)
            continue;

        auto mesh = meshComp->GetMesh();
        if (!mesh)
            continue;

        Renderer::SubmitRenderItem(mesh, entity->GetTransform()->GetWorldTransform());
    }

    Renderer::FlushQueue(cmd, m_defaultPipeline);
}
