#ifndef PIPELINE_CREATION_H
#define PIPELINE_CREATION_H

#include "../Core/GpuHandles.h"
#include "../Core/GpuLimits.h"
#include "../Core/GpuResources.h"

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