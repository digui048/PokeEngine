#include "HierarchyInterface.h"

#include "Poke/Scene/Scene.h"
#include "Poke/Scene/GameObject.h"

#include <imgui.h>

using namespace Poke;

GameObject *HierarchyInterface::s_selectedEntity = nullptr;

void HierarchyInterface::OnInit()
{
}

void HierarchyInterface::OnImGuiRender()
{
    if (!m_isOpen)
        return;

    ImGui::Begin(m_name.c_str(), &m_isOpen);

    const auto root = m_scene->GetRoot();
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
