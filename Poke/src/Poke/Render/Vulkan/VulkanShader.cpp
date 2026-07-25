#include "VulkanShader.h"

#include "Poke/Core/Log.h"
#include <fstream>

using namespace Poke;

std::vector<char> VulkanShader::ReadFile(const std::string &filepath)
{
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        POKE_CORE_CRITICAL("[Vulkan] Failed to open file: {0}", filepath);
        throw std::runtime_error("failed to open file: " + filepath);
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

VkShaderModule Poke::VulkanShader::CreateShaderModule(VkDevice device, const std::vector<char> &code)
{
    VkShaderModuleCreateInfo createinfo{};
    createinfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createinfo.codeSize = code.size();
    createinfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createinfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        POKE_CORE_CRITICAL("[Vulkan] Failed to create shader module!");
    }

    return shaderModule;
}
