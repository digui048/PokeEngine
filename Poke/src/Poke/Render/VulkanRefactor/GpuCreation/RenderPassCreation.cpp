#include "RenderPassCreation.h"

using namespace Poke;

RenderPassCreation &RenderPassCreation::Reset()
{
    renderTargetCount = 0;
    depthStencilTexture = k_invalidTexture;
    resize = 0;
    scaleX = 1.0f;
    scaleY = 1.0f;
    colorOperation = RenderPassOperation::DontCare;
    depthOperation = RenderPassOperation::DontCare;
    stencilOperation = RenderPassOperation::DontCare;
    
    return *this;
}

RenderPassCreation &RenderPassCreation::AddRenderTexture(TextureHandle texture)
{
    outputTextures[renderTargetCount++] = texture;
    return *this;
}

RenderPassCreation &RenderPassCreation::SetDepthStencilTexture(TextureHandle texture)
{
    depthStencilTexture = texture;
    return *this;
}

RenderPassCreation &RenderPassCreation::SetScale(float x, float y, uint8_t resize)
{
    scaleX = x;
    scaleY = y;
    this->resize = resize;
    
    return *this;
}

RenderPassCreation &RenderPassCreation::SetType(RenderPassType::Enum type)
{
    this->type = type;
    return *this;
}

RenderPassCreation &RenderPassCreation::SetOperations(RenderPassOperation::Enum colorOp, RenderPassOperation::Enum depthOp, RenderPassOperation::Enum stencilOp)
{
    colorOperation = colorOp;
    depthOperation = depthOp;
    stencilOperation = stencilOp;

    return *this;
}

RenderPassCreation &RenderPassCreation::SetName(const char *name)
{
    this->name = name;
    return *this;
}
