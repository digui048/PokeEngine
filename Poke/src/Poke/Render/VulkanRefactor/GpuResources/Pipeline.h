#ifndef PIPELINE_RESOURCE_H
#define PIPELINE_RESOURCE_H

#include "../Core/GpuHandles.h"
#include "../Core/GpuLimits.h"
#include "../Core/GpuResources.h"

#include <vulkan/vulkan.h>
#include <cstdint>

namespace Poke
{
    struct DescriptorSetLayout;

    struct Pipeline
    {
        VkPipeline pipeline;
        VkPipelineLayout pipelineLayout;

        VkPipelineBindPoint bindPoint;

        ShaderStateHandle shaderState;

        const DescriptorSetLayout *descriptorSetLayouts[kMaxDescriptorSetLayouts];
        DescriptorSetLayoutHandle descriptorSetLayoutHandles[kMaxDescriptorSetLayouts];
        uint32_t numActiveLayouts = 0;

        DepthStencilCreation depthStencil;
        BlendStateCreation blendState;
        RasterizationCreation rasterization;

        PipelineHandle handle;
        bool isGraphics = true;
    };

    struct PipelineDescription
    {
        ShaderStateHandle shader;
    };
}

#endif