#ifndef GPU_PIPELINE_TYPES_H
#define GPU_PIPELINE_TYPES_H

#include "GpuEnums.h"
#include "GpuLimits.h"

#include <vulkan/vulkan.h>
#include <cstdint>

namespace Poke
{
    struct Rect2D
    {
        float x = 0.0f;
        float y = 0.0f;
        float width = 0.0f;
        float height = 0.0f;
    };

    struct Rect2DInt
    {
        int16_t x = 0;
        int16_t y = 0;
        uint16_t width = 0;
        uint16_t height = 0;
    };

    struct Viewport
    {
        Rect2DInt rect;
        float minDepth = 0.0f;
        float maxDepth = 1.0f;
    };

    struct ViewportState
    {
        uint32_t viewportCount = 0;
        uint32_t scissorCount = 0;

        Viewport *viewports = nullptr;
        Rect2DInt *scissors = nullptr;
    };

    struct StencilOpState
    {
        VkStencilOp fail = VK_STENCIL_OP_KEEP;
        VkStencilOp pass = VK_STENCIL_OP_KEEP;
        VkStencilOp depthFail = VK_STENCIL_OP_KEEP;
        VkCompareOp compare = VK_COMPARE_OP_ALWAYS;
        uint32_t compareMask = 0xff;
        uint32_t writeMask = 0xff;
        uint32_t reference = 0xff;
    };

    struct DepthStencilCreation
    {
        StencilOpState front;
        StencilOpState back;
        VkCompareOp depthCompare = VK_COMPARE_OP_ALWAYS;

        uint8_t depthEnable : 1;
        uint8_t depthWriteEnable : 1;
        uint8_t stencilEnable : 1;
        uint8_t pad : 5;

        DepthStencilCreation() : depthEnable(0), depthWriteEnable(0), stencilEnable(0), pad(0) {}

        DepthStencilCreation &SetDepth(bool write, VkCompareOp comparisonTest);
    };

    struct BlendState
    {
        VkBlendFactor sourceColor = VK_BLEND_FACTOR_ONE;
        VkBlendFactor destinationColor = VK_BLEND_FACTOR_ONE;
        VkBlendOp colorOperation = VK_BLEND_OP_ADD;

        VkBlendFactor sourceAlpha = VK_BLEND_FACTOR_ONE;
        VkBlendFactor destinationAlpha = VK_BLEND_FACTOR_ONE;
        VkBlendOp alphaOperation = VK_BLEND_OP_ADD;

        ColorWriteEnabled::Mask colorWriteMask = ColorWriteEnabled::AllMask;

        uint8_t blendEnable : 1;
        uint8_t separeteBlend : 1;
        uint8_t pad : 6;

        BlendState() : blendEnable(0), separeteBlend(0) {}

        BlendState &SetColor(VkBlendFactor srcColor, VkBlendFactor dstColor, VkBlendOp colorOp);
        BlendState &SetAlpha(VkBlendFactor srcAlpha, VkBlendFactor dstAlpha, VkBlendOp alphaOp);
        BlendState &SetColorWriteMask(ColorWriteEnabled::Mask mask);
    };

    struct BlendStateCreation
    {
        BlendState blendStates[kMaxImageOutputs];
        uint32_t activeStates = 0;

        BlendStateCreation &Reset();
        BlendState &AddBlendState();
    };

    struct RasterizationCreation
    {
        VkCullModeFlagBits cullMode = VK_CULL_MODE_NONE;
        VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        FillMode::Enum fill = FillMode::Solid;
    };

    struct VertexAttribute
    {
        uint16_t location = 0;
        uint16_t binding = 0;
        uint16_t offset = 0;
        VertexComponentFormat::Enum format = VertexComponentFormat::Count;
    };

    struct VertexStream
    {
        uint16_t binding = 0;
        uint16_t stride = 0;
        VertexInputRate::Enum inputRate = VertexInputRate::Count;
    };

    struct VertexInputCreation
    {
        VertexStream streams[kMaxVertexStreams];
        VertexAttribute attributes[kMaxVertexAttributes];

        uint32_t streamCount = 0;
        uint32_t attributeCount = 0;

        VertexInputCreation &Reset();
        VertexInputCreation &AddVertexStream(const VertexStream &stream);
        VertexInputCreation &AddVertexAttribute(const VertexAttribute &attribute);
    };
}

#endif