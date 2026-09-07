#include "EditorModule.h"

#include "Poke/Core/Time.h"

#include "Poke/Render/Renderer.h"

#include "PokeEngineEditor/Interfaces/HierarchyInterface.h"
#include "PokeEngineEditor/Interfaces/MainMenuBarInterface.h"

#include "Poke/Core/Application.h"
#include "Poke/Core/Window.h"

#include "Poke/Render/UniformBuffer.h"
#include "Poke/Render/Vulkan/VulkanPipeline.h"
#include "Poke/Render/Vulkan/VulkanTexture.h"
#include "Poke/Importers/MeshImporter.h"

#include "Poke/Scene/EditorCamera.h"
#include "Poke/Scene/Scene.h"
#include "Poke/Scene/Components/MeshComponent.h"
#include "Poke/Resources/Mesh.h"

using namespace Poke;

EditorModule::EditorModule() = default;
EditorModule::~EditorModule() = default;

void EditorModule::OnInit()
{
    AddInterface<HierarchyInterface>();
    AddInterface<MainMenuBarInterface>();

    m_scene = std::make_unique<Scene>();
    m_scene->OnInit();

    std::shared_ptr<Mesh> shibaMesh = MeshImporter::LoadMesh("Poke/assets/shiba.fbx");

    m_shibaEntity = m_scene->CreateGameObject("Shiba");
    m_meshComp = m_shibaEntity->AddComponent<MeshComponent>(std::move(shibaMesh));

    m_defaultPipeline = Renderer::CreatePipeline("Poke/assets/shaders/defaultShader.vert.spv", "Poke/assets/shaders/defaultShader.frag.spv");
    m_uniformBuffer = std::make_unique<Poke::UniformBuffer>(sizeof(UniformBufferObject));

    m_texture = std::make_shared<VulkanTexture>();
    m_texture->Load("Poke/assets/default_Base_Color.png");

    m_defaultPipeline->SetupDescriptors(m_uniformBuffer.get(), m_texture.get());

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

    UniformBufferObject ubo{};
    ubo.model = glm::mat4(1.0f);
    ubo.view = EditorCamera::Get().GetViewMatrix();
    ubo.proj = EditorCamera::Get().GetProjectionMatrix();

    m_uniformBuffer->SetData(&ubo);
}

void EditorModule::OnRender(VkCommandBuffer cmd)
{
    Renderer::BindPipeline(cmd, m_defaultPipeline);
    Renderer::BindPipelineDescriptors(cmd, m_defaultPipeline);

    m_meshComp->BindMesh(cmd);

    vkCmdDrawIndexed(cmd, static_cast<uint32_t>(m_meshComp->GetMesh()->GetIndices().size()), 1, 0, 0, 0);
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
    m_uniformBuffer.reset();
    m_texture.reset();
    m_defaultPipeline.reset();
}