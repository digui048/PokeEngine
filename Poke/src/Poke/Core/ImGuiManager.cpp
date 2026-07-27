#include "ImGuiManager.h"

#include "Window.h"
#include "Application.h"
#include "Log.h"
#include "Poke/Render/Renderer.h"
#include "Poke/Render/Vulkan/VulkanContext.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_vulkan.h"

using namespace Poke;

ImGuiManager::~ImGuiManager()
{
    Shutdown();
}

void ImGuiManager::Init(Window& window)
{
    VulkanContext& context = Renderer::GetContext();

    VkDescriptorPoolSize poolSizes[] = { { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 } };

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolInfo.maxSets = 1000;
	poolInfo.poolSizeCount = (uint32_t)std::size(poolSizes);
	poolInfo.pPoolSizes = poolSizes;

	if(vkCreateDescriptorPool(context.GetDevice().GetHandle(), &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
    {
        POKE_CORE_ERROR("[ImGui] Failed to create descriptor pool");
        return;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    CustomImGui();

    ImGui_ImplSDL3_InitForVulkan(window.GetSDLWindow());
    
    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance = context.GetInstance().GetHandle();
    initInfo.PhysicalDevice = context.GetPhysicalDevice().GetHandle();
    initInfo.Device = context.GetDevice().GetHandle();
    initInfo.Queue = context.GetDevice().GetGraphicsQueue();
    initInfo.DescriptorPool = m_descriptorPool;
    initInfo.MinImageCount = VulkanSync::MAX_FRAMES_IN_FLIGHT;
    initInfo.ImageCount = static_cast<uint32_t>(context.GetSwapchain().GetImagesViews().size());
    initInfo.QueueFamily = context.GetPhysicalDevice().GetQueueFamilies().GraphicsFamily.value(); 
    initInfo.Queue = context.GetDevice().GetGraphicsQueue();
    initInfo.PipelineCache = VK_NULL_HANDLE;
    initInfo.PipelineInfoMain.Subpass = 0;
    initInfo.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.PipelineInfoMain.RenderPass = context.GetRenderPass().GetHandle();

    ImGui_ImplVulkan_Init(&initInfo);
}

void ImGuiManager::Shutdown()
{
    VulkanContext& context = Renderer::GetContext();

    if (m_descriptorPool != VK_NULL_HANDLE)
    {
        vkDeviceWaitIdle(context.GetDevice().GetHandle());
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();

        vkDestroyDescriptorPool(context.GetDevice().GetHandle(), m_descriptorPool, nullptr);
        m_descriptorPool = VK_NULL_HANDLE;
    }
}

void ImGuiManager::BeginFrame()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::EndFrame(VkCommandBuffer cmdBuffer)
{
    ImGui::Render();
    ImDrawData* drawData = ImGui::GetDrawData();

    if (drawData)
    {
        ImGui_ImplVulkan_RenderDrawData(drawData, cmdBuffer);
    }
}

void Poke::ImGuiManager::CustomImGui()
{
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = "PokeEngineEditor/assets/imgui.ini";
}
