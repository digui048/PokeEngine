#include "HierarchyInterface.h"

#include <imgui.h>

using namespace Poke;

void HierarchyInterface::OnInit()
{
}

void HierarchyInterface::OnImGuiRender()
{
    if (!m_IsOpen)
        return;

    ImGui::Begin(m_Name.c_str(), &m_IsOpen);

    if (ImGui::TreeNode("Main Camera"))
    {
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Testing Cube"))
    {
        ImGui::TreePop();
    }

    ImGui::End();
}
