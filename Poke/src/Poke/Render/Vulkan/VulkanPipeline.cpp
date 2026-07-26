#include "VulkanPipeline.h"

#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanShader.h"
#include "Poke/Core/Log.h"

using namespace Poke;

void VulkanPipeline::Init(VulkanDevice &device, VulkanSwapchain& swapchain, const std::string &vertPath, const std::string &fragPath)
{
    VkDevice logicalDevice = device.GetHandle();

    auto vertCode = VulkanShader::ReadFile(vertPath);
    auto fragCode = VulkanShader::ReadFile(fragPath);

    VkShaderModule vertModule = VulkanShader::CreateShaderModule(logicalDevice, vertCode);
    VkShaderModule fragModule = VulkanShader::CreateShaderModule(logicalDevice, fragCode);

    VkPipelineShaderStageCreateInfo vertStageInfo{};
    vertStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertStageInfo.module = vertModule;
    vertStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragStageInfo{};
    fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragStageInfo.module = fragModule;
    fragStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertStageInfo, fragStageInfo};

    vkDestroyShaderModule(logicalDevice, fragModule, nullptr);
    vkDestroyShaderModule(logicalDevice, vertModule, nullptr);

    POKE_CORE_INFO("[Vulkan] Shader stages configured successfully");
}

void Poke::VulkanPipeline::Shutdown(VulkanDevice &device)
{
    VkDevice logicalDevice = device.GetHandle();

    if (m_pipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(logicalDevice, m_pipeline, nullptr);
        m_pipeline = VK_NULL_HANDLE;
    }

    if (m_pipelineLayout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(logicalDevice, m_pipelineLayout, nullptr);
        m_pipelineLayout = VK_NULL_HANDLE;
    }
}
