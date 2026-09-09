#include "HierarchyInterface.h"

#include "Poke/Scene/Scene.h"
#include "Poke/Scene/GameObject.h"

#include <imgui.h>

using namespace Poke;

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

    ImGui::End();
}

void HierarchyInterface::DrawEntityNode(GameObject *entity)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (entity->GetChildren().empty())
    {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    bool expanded = ImGui::TreeNodeEx((void *)entity, flags, "%s", entity->GetName().c_str());
    if (expanded)
    {
        for (const auto &child : entity->GetChildren())
        {
            DrawEntityNode(child.get());
        }
        ImGui::TreePop();
    }
}
