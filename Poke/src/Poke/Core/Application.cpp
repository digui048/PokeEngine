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

    Renderer::Init();

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f};

    unsigned int indices[] = {0, 1, 2};

    VertexArray vao;
    VertexBuffer *vbo = new VertexBuffer((void *)vertices, (unsigned int)sizeof(vertices));
    VertexBufferLayout layout;
    layout.Push<float>(3);
    vbo->SetLayout(layout);
    IndexBuffer *ebo = new IndexBuffer(indices, 3);

    vao.Bind();
    vbo->Bind();
    ebo->Bind();
    vao.AddBuffer(vbo, ebo);

    Shader *m_shader = new Shader("PokeEngineEditor/assets/shaders/basic.shader");
    m_shader->Bind();

    vao.Unbind();
    vbo->Unbind();
    ebo->Unbind();
    m_shader->Unbind();

    while (m_Running)
    {
        Time::Update();

        PollEvents();

        OnUpdate(Time::DeltaTime());

        Renderer::Clear();

        m_shader->Bind();
        vao.Bind();
        ebo->Bind();

        glDrawElements(GL_TRIANGLES, ebo->GetCount(), GL_UNSIGNED_INT, nullptr);

        m_imguiManager->BeginFrame();

        ImGui::ShowDemoWindow();

        m_imguiManager->EndFrame();

        m_window->SwapWindow();
    }

    delete m_shader;
    delete vbo;
    delete ebo;

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
