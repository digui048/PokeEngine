#ifndef GPU_RENDER_PASS_TYPES_H
#define GPU_RENDER_PASS_TYPES_H

#include "GpuEnums.h"
#include "GpuLimits.h"

#include <vulkan/vulkan.h>
#include <cstdint>

namespace Poke
{
    struct RenderPassOutput
    {
        VkFormat colorFormats[kMaxImageOutputs];
        VkFormat depthStencilFormat;
        uint32_t colorFormatCount;

        RenderPassOperation::Enum colorOperation = RenderPassOperation::DontCare;
        RenderPassOperation::Enum depthOperation = RenderPassOperation::DontCare;
        RenderPassOperation::Enum stencilOperation = RenderPassOperation::DontCare;

        RenderPassOutput &Reset();
        RenderPassOutput &SetColor(VkFormat format);
        RenderPassOutput &SetDepthStencil(VkFormat format);
        RenderPassOutput &SetOperations(RenderPassOperation::Enum operation, RenderPassOperation::Enum depthOperation, RenderPassOperation::Enum stencilOperation);
    };
}

#endif