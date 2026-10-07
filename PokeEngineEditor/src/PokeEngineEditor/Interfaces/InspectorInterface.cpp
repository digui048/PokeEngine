#include "InspectorInterface.h"
#include "HierarchyInterface.h"
#include "Poke/Scene/GameObject.h"
#include "Poke/Scene/Components/TransformComponent.h"
#include "Poke/Scene/Components/MeshRendererComponent.h"
#include "Poke/Utils/ImGuiUtils.h"

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
                DrawTransformComponent(static_cast<TransformComponent *>(component.get()));
            }
            else if (component->GetType() == ComponentType::MESH_RENDERER)
            {
                DrawMeshComponent(static_cast<MeshRendererComponent *>(component.get()));
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
    if (ImGui::CollapsingHeader("MeshRenderer", ImGuiTreeNodeFlags_DefaultOpen))
    {
        bool hasMesh = (mesh->GetMesh() != nullptr);

        ImGui::Text("Mesh Asset:");
        ImGui::SameLine();

        float xButtonWidth = ImGui::GetFrameHeight();
        float slotWidth = hasMesh ? (ImGui::GetContentRegionAvail().x - xButtonWidth - ImGui::GetStyle().ItemSpacing.x) : ImGui::GetContentRegionAvail().x;

        if (!hasMesh)
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));

        std::string meshLabel = hasMesh ? "Mesh Assigned" : "None (Mesh)";
        ImGui::Button(meshLabel.c_str(), ImVec2(slotWidth, 0.0f));

        if (!hasMesh)
            ImGui::PopStyleColor();

        AssetHandle droppedHandle = 0;
        if (AcceptDragDropTargetPayload(PAYLOAD_ASSET_HANDLE, droppedHandle))
        {
            if (static_cast<uint64_t>(droppedHandle) != 0)
            {
                std::shared_ptr<Asset> asset = m_assetManager->GetAsset(droppedHandle);
                std::shared_ptr<Mesh> newMesh = std::dynamic_pointer_cast<Mesh>(asset);

                if (newMesh)
                    mesh->SetMesh(newMesh);
            }
        }

        if (hasMesh)
        {
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.15f, 0.15f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.25f, 0.25f, 0.8f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.35f, 0.35f, 1.0f));
        
            if (ImGui::Button("X##ClearMesh", ImVec2(xButtonWidth, 0.0f)))
                mesh->SetMesh(nullptr);

            ImGui::PopStyleColor(3);

            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Clear / Remove Mesh");
        }

        ImGui::Separator();
        ImGui::Text("Vertices: %zu", mesh->GetVerticesCount());
        ImGui::Text("Indices: %zu", mesh->GetIndicesCount());
        ImGui::Text("Triangles: %zu", mesh->GetIndicesCount() / 3);
    }
}
