#include "EditorModule.h"

#include "Poke/Core/Time.h"

#include "Poke/Render/Shader.h"
#include "Poke/Render/VertexArray.h"
#include "Poke/Render/VertexArray.h"
#include "Poke/Render/VertexBuffer.h"
#include "Poke/Render/IndexBuffer.h"
#include "Poke/Render/Renderer.h"

#include "PokeEngineEditor/Interfaces/HierarchyInterface.h"
#include "PokeEngineEditor/Interfaces/MainMenuBarInterface.h"

#include "Poke/Importer/MeshImporter.h"
#include "Poke/Resources/Mesh.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>

#include <vulkan/vulkan.h>

using namespace Poke;

EditorModule::EditorModule() = default;
EditorModule::~EditorModule() = default;

void EditorModule::OnInit()
{
    //AddInterface<HierarchyInterface>();
    //AddInterface<MainMenuBarInterface>();
}

void EditorModule::OnUpdate(float dt)
{
    for (auto &interface : m_Interfaces)
    {
        if (interface->IsOpen())
        {
            interface->OnUpdate(dt);
        }
    }
}

void EditorModule::OnImGuiRender()
{
    for (auto &interface : m_Interfaces)
    {
        interface->OnImGuiRender();
    }
}

void EditorModule::OnShutdown()
{
    for(auto&interface : m_Interfaces)
    {
        interface->OnShutdown();
    }
    m_Interfaces.clear();
}