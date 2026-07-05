#include "Application.h"
#include "Window.h"
#include "ImGuiManager.h"
#include "Log.h"
#include "Time.h"
#include "Module.h"

#include "Poke/Render/Renderer.h"

#include <glad/glad.h>

using namespace Poke;

Application *Application::s_Instance = nullptr;

Application::Application()
{
    Log::Init();

    POKE_CORE_INFO("Starting PokeEngine");

    s_Instance = this;

    m_window = std::make_unique<Window>("PokeEngine", WINDOW_PREV_WIDTH, WINDOW_PREV_HEIGHT);

    Time::Init();

    m_imguiManager = std::make_unique<ImGuiManager>();
}

Application::~Application() = default;

Application &Application::GetInstance()
{
    return *s_Instance;
}

void Application::Run()
{
    m_imguiManager->Init();

    OnInit();

    Renderer::SetClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    while (m_Running)
    {
        Time::Update();

        PollEvents();

        Renderer::Clear();

        OnUpdate(Time::DeltaTime());

        for (auto &module : m_modules)
        {
            module->OnUpdate(Time::DeltaTime());
        }

        m_imguiManager->BeginFrame();

        for (auto &module : m_modules)
        {
            module->OnImGuiRender();
        }

        m_imguiManager->EndFrame();

        m_window->SwapWindow();
    }

    ClearModules();
    OnShutdown();

    m_imguiManager.reset();
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
            int w, h;
            m_window->GetWindowSize(w, h);
            glViewport(0, 0, w, h);
            
        default:
            break;
        }
    }
}
