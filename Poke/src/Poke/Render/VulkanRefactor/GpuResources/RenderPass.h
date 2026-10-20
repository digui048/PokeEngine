#ifndef RENDER_PASS_RESOURCE_H
#define RENDER_PASS_RESOURCE_H

#include "../Core/GpuEnums.h"
#include "../Core/GpuHandles.h"
#include "../Core/GpuLimits.h"
#include "../Core/GpuResources.h"

#include <vulkan/vulkan.h>
#include <cstdint>

namespace Poke
{
    struct RenderPass
    {
        VkRenderPass renderPass;
        VkFramebuffer framebuffer;

        RenderPassOutput output;

        TextureHandle outputTextures[kMaxImageOutputs];
        TextureHandle depthTexture{};

        RenderPassType::Enum type;

        float scaleX = 1.0f;
        float scaleY = 1.0f;
        uint16_t width = 0;
        uint16_t height = 0;
        uint16_t dispatchX = 0;
        uint16_t dispatchY = 0;
        uint16_t dispatchZ = 0;

        uint8_t resize = 0;
        uint8_t renderTargetCount = 0;

        const char *name = nullptr;
    };
}

#endif