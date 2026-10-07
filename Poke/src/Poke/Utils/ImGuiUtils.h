#ifndef IMGUI_UTILS_H
#define IMGUI_UTILS_H

#include <imgui.h>
#include <string>

namespace Poke
{
    inline constexpr const char* PAYLOAD_ASSET_HANDLE = "POKE_ASSET_HANDLE";

    template<typename T>
    inline bool BeginDragDropSourcePayload(const char* type, const T& data, const char* tooltipText = nullptr)
    {
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
        {
            ImGui::SetDragDropPayload(type, &data, sizeof(T));
            if (tooltipText)
                ImGui::TextUnformatted(tooltipText);
            
            ImGui::EndDragDropSource();
            return true;
        }
        return false;
    }

    template<typename T>
    inline bool AcceptDragDropTargetPayload(const char* type, T& outData)
    {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(type))
            {
                if (payload->DataSize == sizeof(T))
                {
                    outData = *static_cast<const T*>(payload->Data);
                    ImGui::EndDragDropTarget();
                    return true;
                }
            }
            ImGui::EndDragDropTarget();
        }
        return false;
    }
}

#endif