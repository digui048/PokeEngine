#include "Application.h"
#include "Window.h"

using namespace Poke;

Application *Application::s_Instance = nullptr;

Application::Application()
{
    m_window = std::make_unique<Window>("PokeEngine", WINDOW_PREV_WIDTH, WINDOW_PREV_HEIGHT);
}

Application::~Application() = default;

Application &Application::GetInstance()
{
    return *s_Instance;
}

void Application::Run()
{
    OnInit();

    while (m_Running)
    {
        float dt = 0.016f;
        OnUpdate(dt);
    }

    OnShutdown();
}