#ifndef SHADER_CREATION_H
#define SHADER_CREATION_H

#include "../Core/GpuLimits.h"

#include <vulkan/vulkan.h>
#include <cstdint>

namespace Poke
{
    struct ShaderStage
    {
        const char *code = nullptr;
        uint32_t codeSize = 0;
        VkShaderStageFlagBits stage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
    };

    struct ShaderStateCreation
    {
        ShaderStage stages[kMaxShaderStages];

        const char *name = nullptr;

        uint32_t stagesCount = 0;
        uint32_t spvInput = 0;

        ShaderStateCreation &Reset();
        ShaderStateCreation &AddStage(const char *code, uint32_t codeSize, VkShaderStageFlagBits type);
        ShaderStateCreation &SetSPVInput(bool value);
        ShaderStateCreation &SetName(const char *name);
    };
}

#endif