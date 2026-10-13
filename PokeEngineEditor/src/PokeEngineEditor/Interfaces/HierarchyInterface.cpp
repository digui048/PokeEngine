#include "HierarchyInterface.h"

#include "Poke/Scene/Scene.h"
#include "Poke/Scene/GameObject.h"

#include <imgui.h>
#include <utility>

using namespace Poke;

std::shared_ptr<Scene> HierarchyInterface::s_activeScene = nullptr;
GameObject *HierarchyInterface::s_selectedEntity = nullptr;

void HierarchyInterface::OnInit()
{
}

void HierarchyInterface::OnImGuiRender()
{
    if (!m_isOpen)
        return;

    ImGui::Begin(m_name.c_str(), &m_isOpen);

    if (!s_activeScene || !s_activeScene->GetRoot())
    {
        ImGui::End();
        return;
    }

    const auto root = s_activeScene->GetRoot();
    for (const auto &object : root->GetChildren())
    {
        DrawEntityNode(object.get());
    }

    if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
    {
        s_selectedEntity = nullptr;
    }

    ImGui::End();
}

void HierarchyInterface::SetActiveScene(std::shared_ptr<Scene> scene)
{
    s_selectedEntity = nullptr;
    s_activeScene = std::move(scene);
}

void HierarchyInterface::DrawEntityNode(GameObject *entity)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (s_selectedEntity == entity)
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    if (entity->GetChildren().empty())
    {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    bool expanded = ImGui::TreeNodeEx((void *)entity, flags, "%s", entity->GetName().c_str());

    if (ImGui::IsItemClicked())
    {
        s_selectedEntity = entity;
    }

    if (expanded)
    {
        for (const auto &child : entity->GetChildren())
        {
            DrawEntityNode(child.get());
        }
        ImGui::TreePop();
    }
}
