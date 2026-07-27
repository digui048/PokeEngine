#ifndef IMGUI_MANAGER_H
#define IMGUI_MANAGER_H

#include <vulkan/vulkan.h>

namespace Poke
{
    class Window;

    class ImGuiManager
    {
    public:
        ImGuiManager() = default;
        ~ImGuiManager();

        void Init(Window& window);
        void Shutdown();

        void BeginFrame();
        void EndFrame(VkCommandBuffer cmdBuffer);

    private:
        void CustomImGui();

    private:
        VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    };
}

#endif