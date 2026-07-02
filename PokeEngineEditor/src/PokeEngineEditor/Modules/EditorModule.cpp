#include "EditorModule.h"

#include "Poke/Core/Time.h"

#include "Poke/Render/Shader.h"
#include "Poke/Render/VertexArray.h"
#include "Poke/Render/VertexArray.h"
#include "Poke/Render/VertexBuffer.h"
#include "Poke/Render/IndexBuffer.h"
#include "Poke/Render/Renderer.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>

using namespace Poke;

EditorModule::EditorModule() = default;
EditorModule::~EditorModule() = default;

void EditorModule::OnInit()
{
    float vertices[] = {
        // Front face
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,

        // Back face
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,

        // Left Face
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,

        // Right Face
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,

        // Top Face
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,

        // Bottom Face
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f};

    unsigned int indices[] = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20};

    m_vao = std::make_unique<VertexArray>();

    m_vbo = std::make_shared<VertexBuffer>((void *)vertices, (unsigned int)sizeof(vertices));

    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(3);
    layout.Push<float>(2);
    m_vbo->SetLayout(layout);

    m_ebo = std::make_shared<IndexBuffer>(indices, 36);

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
}

void EditorModule::OnUpdate(float dt)
{
    float time = Time::TotalTime();

    glm::mat4 model = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.5f, 1.0f, 0.0f));

    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));

    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

    glm::mat4 mvp = proj * view * model;

    m_shader->Bind();
    m_shader->SetUniformMatrix4f("u_MVP", mvp);

    Renderer::DrawIndexed(*m_vao, *m_shader);
}

void EditorModule::OnImGuiRender()
{
    ImGui::Begin("Hierarchy");
    ImGui::Text("Scene nodes");
    ImGui::End();

    ImGui::ShowDemoWindow();
}

void EditorModule::OnShutdown()
{
}