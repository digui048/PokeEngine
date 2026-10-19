#ifndef PIPELINE_CREATION_H
#define PIPELINE_CREATION_H

#include "../GpuEnums.h"
#include "../GpuHandles.h"
#include "../GpuLimits.h"
#include "../GpuPipelineTypes.h"
#include "../GpuRenderPassTypes.h"

#include "ShaderCreation.h"

#include <cstdint>

namespace Poke
{
    struct PipelineCreation
    {
        RasterizationCreation rasterization;
        DepthStencilCreation depthStencil;
        BlendStateCreation blendState;
        VertexInputCreation vertexInput;
        ShaderStateCreation shaders;

        RenderPassOutput renderPass;
        DescriptorSetLayoutHandle descriptorSetLayouts[kMaxDescriptorSetLayouts];
        const ViewportState *viewportState = nullptr;

        uint32_t activeLayoutsCount = 0;

        const char *name = nullptr;

        PipelineCreation &AddDescriptorSetLayout(DescriptorSetLayoutHandle layout);
        RenderPassOutput &GetRenderPassOutput();
    };
}

#endif