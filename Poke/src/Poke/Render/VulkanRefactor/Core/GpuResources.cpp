#include "GpuResources.h"

using namespace Poke;

DepthStencilCreation &DepthStencilCreation::SetDepth(bool write, VkCompareOp comparisonTest)
{
    depthWriteEnable = write;
    depthCompare = comparisonTest;
    depthEnable = 1;

    return *this;
}

BlendState &BlendState::SetColor(VkBlendFactor srcColor, VkBlendFactor dstColor, VkBlendOp colorOp)
{
    sourceColor = srcColor;
    destinationColor = dstColor;
    colorOperation = colorOp;
    blendEnable = 1;

    return *this;
}

BlendState &BlendState::SetAlpha(VkBlendFactor srcAlpha, VkBlendFactor dstAlpha, VkBlendOp alphaOp)
{
    sourceAlpha = srcAlpha;
    destinationAlpha = dstAlpha;
    alphaOperation = alphaOp;
    separeteBlend = 1;

    return *this;
}

BlendState &BlendState::SetColorWriteMask(ColorWriteEnabled::Mask mask)
{
    colorWriteMask = mask;
    return *this;
}

BlendStateCreation &BlendStateCreation::Reset()
{
    activeStates = 0;
    return *this;
}

BlendState &BlendStateCreation::AddBlendState()
{
    return blendStates[activeStates++];
}

VertexInputCreation &VertexInputCreation::Reset()
{
    streamCount = 0;
    attributeCount = 0;

    return *this;
}

VertexInputCreation &VertexInputCreation::AddVertexStream(const VertexStream &stream)
{
    streams[streamCount++] = stream;
    return *this;
}

VertexInputCreation &VertexInputCreation::AddVertexAttribute(const VertexAttribute &attribute)
{
    attributes[attributeCount++] = attribute;
    return *this;
}

RenderPassOutput &RenderPassOutput::Reset()
{
    colorFormatCount = 0;
    for (uint32_t i = 0; i < kMaxImageOutputs; ++i)
    {
        colorFormats[i] = VK_FORMAT_UNDEFINED;
    }
    depthStencilFormat = VK_FORMAT_UNDEFINED;
    colorOperation = RenderPassOperation::DontCare;
    depthOperation = RenderPassOperation::DontCare;
    stencilOperation = RenderPassOperation::DontCare;

    return *this;
}

RenderPassOutput &RenderPassOutput::SetColor(VkFormat format)
{
    colorFormats[colorFormatCount++] = format;
    return *this;
}

RenderPassOutput &RenderPassOutput::SetDepthStencil(VkFormat format)
{
    depthStencilFormat = format;
    return *this;
}

RenderPassOutput &RenderPassOutput::SetOperations(RenderPassOperation::Enum operation, RenderPassOperation::Enum depthOperation, RenderPassOperation::Enum stencilOperation)
{
    colorOperation = operation;
    depthOperation = depthOperation;
    stencilOperation = stencilOperation;
    
    return *this;
}
