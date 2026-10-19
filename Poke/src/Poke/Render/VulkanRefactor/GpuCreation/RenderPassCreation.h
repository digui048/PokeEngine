#ifndef RENDER_PASS_CREATION_H
#define RENDER_PASS_CREATION_H

#include "../GpuEnums.h"
#include "../GpuHandles.h"
#include "../GpuLimits.h"

#include <cstdint>

namespace Poke
{
    struct RenderPassCreation
    {
        uint16_t renderTargetCount = 0;
        RenderPassType::Enum type = RenderPassType::Geometry;

        TextureHandle outputTextures[kMaxImageOutputs];
        TextureHandle depthStencilTexture;

        float scaleX = 1.0f;
        float scaleY = 1.0f;
        uint8_t resize = 1;

        RenderPassOperation::Enum colorOperation = RenderPassOperation::DontCare;
        RenderPassOperation::Enum depthOperation = RenderPassOperation::DontCare;
        RenderPassOperation::Enum stencilOperation = RenderPassOperation::DontCare;

        const char *name = nullptr;

        RenderPassCreation &Reset();
        RenderPassCreation &AddRenderTexture(TextureHandle texture);
        RenderPassCreation &SetDepthStencilTexture(TextureHandle texture);
        RenderPassCreation &SetScale(float x, float y, uint8_t resize);
        RenderPassCreation &SetType(RenderPassType::Enum type);
        RenderPassCreation &SetOperations(RenderPassOperation::Enum colorOp, RenderPassOperation::Enum depthOp, RenderPassOperation::Enum stencilOp);
        RenderPassCreation &SetName(const char *name);
    };
}

#endif