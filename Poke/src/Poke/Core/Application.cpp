#include "Application.h"
#include "Window.h"
#include "ImGuiManager.h"
#include "Log.h"
#include "Time.h"
#include "Module.h"

#include "Poke/Render/Renderer.h"
#include "Poke/Render/VertexBuffer.h"
#include "Poke/Render/IndexBuffer.h"
#include "Poke/Render/UniformBuffer.h"
#include "Poke/Render/Vulkan/VulkanPipeline.h"
#include "Poke/Render/Vulkan/VulkanTexture.h"

#include "imgui_impl_sdl3.h"

using namespace Poke;

Application *Application::s_Instance = nullptr;

Application::Application()
{
    Log::Init();

    POKE_CORE_INFO("Starting PokeEngine");

    s_Instance = this;

    m_window = std::make_unique<Window>("PokeEngine", WINDOW_PREV_WIDTH, WINDOW_PREV_HEIGHT);

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

    std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{ 0.5f, -0.5,  0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
    };

    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6 ,7, 4
    };

    m_defaultPipeline = Renderer::CreatePipeline("Poke/assets/shaders/defaultShader.vert.spv", "Poke/assets/shaders/defaultShader.frag.spv");
    m_vertexBuffer = std::make_unique<Poke::VertexBuffer>(vertices);
    m_indexBuffer = std::make_unique<Poke::IndexBuffer>(indices);
    m_uniformBuffer = std::make_unique<Poke::UniformBuffer>(sizeof(UniformBufferObject));

    m_texture = std::make_shared<VulkanTexture>();
    m_texture->Load("Poke/assets/test.png");

    m_defaultPipeline->SetupDescriptors(m_uniformBuffer.get(), m_texture.get());

    Renderer::SetClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    while (m_Running)
    {
        Time::Update();

        PollEvents();

        static float time = 0.0f;
        time += Time::DeltaTime();

        UniformBufferObject ubo{};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        int w;
        int h;
        m_window->GetWindowSize(w, h);
        float aspect = static_cast<float>(w) / static_cast<float>(h);
        ubo.proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 10.0f);
        ubo.proj[1][1] *= -1;

        m_uniformBuffer->SetData(&ubo);

        VkCommandBuffer cmd = Renderer::BeginFrame(*m_window);
        if (cmd != VK_NULL_HANDLE)
        {
            Renderer::BindPipeline(cmd, m_defaultPipeline);
            Renderer::BindPipelineDescriptors(cmd, m_defaultPipeline);

            m_vertexBuffer->Bind(cmd);
            m_indexBuffer->Bind(cmd);

            vkCmdDrawIndexed(cmd, m_indexBuffer->GetIndexCount(), 1, 0, 0, 0);

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

    m_vertexBuffer.reset();
    m_indexBuffer.reset();
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

        default:
            break;
        }
    }
}
