#include "InspectorInterface.h"
#include "HierarchyInterface.h"
#include "Poke/Scene/GameObject.h"
#include "Poke/Scene/Components/TransformComponent.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

using namespace Poke;

void InspectorInterface::OnImGuiRender()
{
    ImGui::Begin("Inspector");

    GameObject *selectedEntity = HierarchyInterface::GetSelectedEntity();

    if (selectedEntity)
    {
        ImGui::Text("%s", selectedEntity->GetName().c_str());
        ImGui::Separator();

        auto *transform = selectedEntity->GetTransform();
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            glm::vec3 pos = transform->GetLocalTranslation();
            glm::vec3 rot = transform->GetLocalRotationEuler();
            glm::vec3 scale = transform->GetLocalScale();

            if (ImGui::DragFloat3("Position", glm::value_ptr(pos), 0.1f))
                transform->SetLocalTranslation(pos);
            if (ImGui::DragFloat3("Rotation", glm::value_ptr(rot), 0.1f))
                transform->SetLocalRotationEuler(rot);
            if (ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.1f))
                transform->SetLocalScale(scale);
        }
    }
    else
    {
        ImGui::TextDisabled("No entity selected");
    }
    ImGui::End();
}