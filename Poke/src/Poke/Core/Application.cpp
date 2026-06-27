#include "Application.h"
#include "Window.h"
#include "ImGuiManager.h"
#include "Log.h"
#include "Time.h"

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

    Renderer::Init();

    while (m_Running)
    {
        Time::Update();

        PollEvents();

        OnUpdate(Time::DeltaTime());

        Renderer::Clear();

        m_imguiManager->BeginFrame();

        ImGui::ShowDemoWindow();

        m_imguiManager->EndFrame();

        m_window->SwapWindow();
    }

    OnShutdown();

    m_imguiManager.reset();
    m_window.reset();

    POKE_CORE_INFO("Engine shutdown");
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
        {
            glViewport(0, 0, sdlEvent.window.data1, sdlEvent.window.data2);
            break;
        }

        default:
            break;
        }
    }
}
