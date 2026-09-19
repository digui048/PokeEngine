#include "Application.h"
#include "Window.h"
#include "ImGuiManager.h"
#include "Log.h"
#include "Time.h"
#include "Input.h"
#include "Module.h"

#include "Poke/Render/Renderer.h"

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

    m_pendingModule = nullptr;
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

    Renderer::SetClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    while (m_Running)
    {
        Time::Update();
        PollEvents();
        Input::Update();

        float dt = Time::DeltaTime();

        OnUpdate(dt);
        for (auto &module : m_modules)
        {
            module->OnUpdate(dt);
        }

        VkCommandBuffer cmd = Renderer::BeginFrame(*m_window);
        if (cmd != VK_NULL_HANDLE)
        {
            for (auto &module : m_modules)
            {
                module->OnRender(cmd);
            }

            m_imguiManager->BeginFrame();

            for (auto &module : m_modules)
            {
                module->OnImGuiRender();
            }

            m_imguiManager->EndFrame(cmd);

            Renderer::EndFrame(*m_window);
        }

        if (m_pendingModule)
        {
            Renderer::WaitIdle();
            
            ClearModules();
            PushModule(m_pendingModule);

            m_pendingModule = nullptr;
        }
    }

    Renderer::WaitIdle();

    ClearModules();
    OnShutdown();

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

void Application::ForceQuit()
{
    m_Running = false;
}

void Application::SwitchModule(std::shared_ptr<Module> newModule)
{
    m_pendingModule = newModule;
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
