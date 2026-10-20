#ifndef SHADER_RESOURCE_H
#define SHADER_RESOURCE_H

#include "../Core/GpuLimits.h"

#include <vulkan/vulkan.h>

namespace Poke
{
    struct ShaderState
    {
        VkPipelineShaderStageCreateInfo stageInfos[kMaxShaderStages];

        const char *name = nullptr;

        uint32_t activeShaders = 0;
        bool isGraphicsPipeline = false;
    };

    struct ShaderStateDescription
    {
        void *nativeHandle = nullptr;
        const char *name = nullptr;
    };

}

#endif