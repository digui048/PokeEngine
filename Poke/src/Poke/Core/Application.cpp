#include "Application.h"
#include "Window.h"
#include "ImGuiManager.h"
#include "Log.h"

// OpenGL
#include <glad/glad.h>
#include <SDL3/SDL.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Setup VS and PS in GLSL
const char *vertexShaderSource = "\n"
                                 "#version 460 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";

const char *fragmentShaderSource = "\n"
                                   "#version 460 core\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
                                   "}\0";

using namespace Poke;

Application *Application::s_Instance = nullptr;

Application::Application()
{
    Log::Init();
    m_window = std::make_unique<Window>("PokeEngine", WINDOW_PREV_WIDTH, WINDOW_PREV_HEIGHT);
    m_imguiManager = std::make_unique<ImGuiManager>();
    s_Instance = this;
}

Application::~Application() = default;

Application &Application::GetInstance()
{
    return *s_Instance;
}

void Application::Run()
{
    OnInit();

    // Init all OpenGL function pointers at runtime (not linked at compile time)
    gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

    // Used for mapping NDC coordinates (-1.0f to 1.0f) to pixel coordinates (e.g. 1920x1080)
    glViewport(0, 0, 1920, 980);

    // Create & compile vertex and fragment shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Create Program and bind shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Delete shaders since we've created a program already and they are contained there
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Create Triangle Data
    GLfloat vertices[] =
        {
            -0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,
            0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,
            0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f,
            -0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f,
            0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f,
            0.0f, -0.5f * float(sqrt(3)) / 3, 0.0f};

    GLuint indices[] =
        {
            0, 3, 5,
            3, 2, 4,
            5, 4, 1};

    // Create VAO & VBO & EBO
    GLuint VAO;
    glGenVertexArrays(1, &VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);

    GLuint EBO;
    glGenBuffers(1, &EBO);

    // Bind VAO and VBO
    glBindVertexArray(VAO);

    // Link GL_ARRAY_BUFFER to vertices data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Link GL_ELEMENT_ARRAY_BUFFER to indices data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Define Vertex layout and set attribute index
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Unlink VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    while (m_Running)
    {
        float dt = 0.016f;
        OnUpdate(dt);

        // INPUT
        // SDL EVENTS
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
            {
                m_Running = false;
            }

            if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_RIGHT)
            {
            }

            if ((event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_RIGHT))
            {
            }
        }
        // RENDER
        // Clear screen color
        glClearColor(0.1f, 0.2f, 0.2f, 1.0f);

        // Clear Color Buffer and Depth Buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // RENDER
        // Use shader program & bind VAO
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        // Specify primitive type and vertex count
        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

        m_imguiManager->BeginFrame();

        ImGui::ShowDemoWindow();

        m_imguiManager->EndFrame();

        m_window->SwapWindow();
    }

    // Delete VAO, VBO and shader program
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    OnShutdown();
}

void Poke::Application::OnInit()
{
    m_imguiManager->Init();
}

void Poke::Application::OnUpdate(float dt)
{
}

void Poke::Application::OnShutdown()
{
}
