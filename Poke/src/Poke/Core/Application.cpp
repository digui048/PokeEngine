#include "Application.h"
#include "Window.h"
#include "ImGuiManager.h"
#include "Log.h"
#include "Time.h"
#include "Input.h"
#include "Module.h"

#include "Poke/Render/Renderer.h"
#include "Poke/Render/UniformBuffer.h"
#include "Poke/Render/Vulkan/VulkanPipeline.h"
#include "Poke/Render/Vulkan/VulkanTexture.h"
#include "Poke/Importers/MeshImporter.h"

#include "Poke/Scene/EditorCamera.h"

#include "imgui_impl_sdl3.h"

using namespace Poke;

Application *Application::s_Instance = nullptr;

Application::Application()
{
    Log::Init();

    POKE_CORE_INFO("Starting PokeEngine");

    s_Instance = this;

    m_window = std::make_unique<Window>("PokeEngine", WINDOW_PREV_WIDTH, WINDOW_PREV_HEIGHT);

    Input::Init(m_window->GetSDLWindow());

    Time::Init();

    Renderer::Init(*m_window);

    m_imguiManager = std::make_unique<ImGuiManager>();
}

Application::~Application() = default;

Application &Application::GetInstance()
{
    return *s_Instance;
}

void Application::Run()
{
    m_imguiManager->Init(*m_window);

    OnInit();

    m_mesh = MeshImporter::LoadMesh("Poke/assets/shiba.fbx");
    m_defaultPipeline = Renderer::CreatePipeline("Poke/assets/shaders/defaultShader.vert.spv", "Poke/assets/shaders/defaultShader.frag.spv");
    m_uniformBuffer = std::make_unique<Poke::UniformBuffer>(sizeof(UniformBufferObject));

    m_texture = std::make_shared<VulkanTexture>();
    m_texture->Load("Poke/assets/default_Base_Color.png");

    m_defaultPipeline->SetupDescriptors(m_uniformBuffer.get(), m_texture.get());

    Renderer::SetClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    int w;
    int h;
    m_window->GetWindowSize(w, h);
    EditorCamera::Get().Init(w, h);

    while (m_Running)
    {
        Time::Update();
        PollEvents();
        Input::Update();

        EditorCamera::Get().OnUpdate(Time::DeltaTime());
        m_window->GetWindowSize(w, h);
        EditorCamera::Get().Resize(w, h);

        UniformBufferObject ubo{};
        ubo.model = glm::mat4(1.0f);
        ubo.view = EditorCamera::Get().GetViewMatrix();
        ubo.proj = EditorCamera::Get().GetProjectionMatrix();

        m_uniformBuffer->SetData(&ubo);

        VkCommandBuffer cmd = Renderer::BeginFrame(*m_window);
        if (cmd != VK_NULL_HANDLE)
        {
            Renderer::BindPipeline(cmd, m_defaultPipeline);
            Renderer::BindPipelineDescriptors(cmd, m_defaultPipeline);

            m_mesh->Bind(cmd);

            vkCmdDrawIndexed(cmd, static_cast<uint32_t>(m_mesh->GetIndices().size()), 1, 0, 0, 0);

            m_imguiManager->BeginFrame();

            for (auto &module : m_modules)
            {
                module->OnImGuiRender();
            }

            m_imguiManager->EndFrame(cmd);

            Renderer::EndFrame(*m_window);
        }

        OnUpdate(Time::DeltaTime());

        for (auto &module : m_modules)
        {
            module->OnUpdate(Time::DeltaTime());
        }
    }

    ClearModules();
    OnShutdown();

    Renderer::WaitIdle();

    m_mesh.reset();
    m_uniformBuffer.reset();
    m_texture.reset();
    m_defaultPipeline.reset();

    m_imguiManager.reset();
    Renderer::Shutdown();

    m_window.reset();

    POKE_CORE_INFO("Engine shutdown");
}

void Application::PushModule(std::shared_ptr<Module> module)
{
    m_modules.push_back(module);
    module->OnInit();
}

void Application::ClearModules()
{
    for (auto &module : m_modules)
    {
        module->OnShutdown();
    }
    m_modules.clear();
}

void Poke::Application::ForceQuit()
{
    m_Running = false;
}

void Application::PollEvents()
{
    SDL_Event sdlEvent;

    while (SDL_PollEvent(&sdlEvent))
    {
        ImGui_ImplSDL3_ProcessEvent(&sdlEvent);

        switch (sdlEvent.type)
        {
        case SDL_EVENT_QUIT:
            m_Running = false;
            break;

        case SDL_EVENT_WINDOW_RESIZED:
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            Renderer::FrameResized();
            break;
        
        case SDL_EVENT_MOUSE_WHEEL:
            EditorCamera::Get().OnMouseScroll(sdlEvent.wheel.y);
            break;

        default:
            break;
        }
    }
}
