#include "Application.h"
#include "Window.h"
#include "ImGuiManager.h"
#include "Log.h"
#include "Time.h"

#include "Poke/Render/Shader.h"
#include "Poke/Render/VertexArray.h"
#include "Poke/Render/VertexArray.h"
#include "Poke/Render/VertexBuffer.h"
#include "Poke/Render/IndexBuffer.h"
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

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f};

    unsigned int indices[] = {0, 1, 2};

    m_vao = std::make_unique<VertexArray>();

    m_vbo = std::make_shared<VertexBuffer>((void *)vertices, (unsigned int)sizeof(vertices));

    VertexBufferLayout layout;
    layout.Push<float>(3);
    m_vbo->SetLayout(layout);
    m_ebo = std::make_shared<IndexBuffer>(indices, 3);

    m_vao->Bind();
    m_vbo->Bind();
    m_ebo->Bind();
    m_vao->AddBuffer(m_vbo.get(), m_ebo.get());

    m_shader = std::make_shared<Shader>("PokeEngineEditor/assets/shaders/basic.shader");

    m_shader->Bind();

    m_vao->Unbind();
    m_vbo->Unbind();
    m_ebo->Unbind();
    m_shader->Unbind();

    Renderer::SetClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    while (m_Running)
    {
        Time::Update();

        PollEvents();

        OnUpdate(Time::DeltaTime());

        Renderer::Clear();
        
        Renderer::DrawIndexed(*m_vao, *m_shader);

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
