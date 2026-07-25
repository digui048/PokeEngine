#ifndef VULKAN_SHADER_H
#define VULKAN_SHADER_H

#include <vulkan/vulkan.h>
#include <string>
#include <vector>

namespace Poke
{
    class VulkanShader
    {
    public:
        static std::vector<char> ReadFile(const std::string& filepath);
        static VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char>& code);
    };
}

#endif