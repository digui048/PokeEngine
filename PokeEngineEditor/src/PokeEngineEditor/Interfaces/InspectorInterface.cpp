#include "InspectorInterface.h"
#include "HierarchyInterface.h"
#include "Poke/Scene/GameObject.h"
#include "Poke/Scene/Components/TransformComponent.h"
#include "Poke/Scene/Components/MeshRendererComponent.h"

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

        for (auto &component : selectedEntity->GetComponents())
        {
            if (component->GetType() == ComponentType::TRANSFORM)
            {
                DrawTransformComponent(static_cast<TransformComponent*>(component.get()));
            }
            else if (component->GetType() == ComponentType::MESH)
            {
                DrawMeshComponent(static_cast<MeshRendererComponent*>(component.get()));
            }
        }
    }
    else
    {
        ImGui::TextDisabled("No entity selected");
    }
    ImGui::End();
}

void InspectorInterface::DrawTransformComponent(TransformComponent *transform)
{
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

void InspectorInterface::DrawMeshComponent(MeshRendererComponent *mesh)
{
    if (ImGui::CollapsingHeader("MeshComponent", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Vertices: %zu", mesh->GetVerticesCount());
        ImGui::Text("Indices: %zu", mesh->GetIndicesCount());
        ImGui::Text("Triangles: %zu", mesh->GetIndicesCount()/ 3);
    }
}
