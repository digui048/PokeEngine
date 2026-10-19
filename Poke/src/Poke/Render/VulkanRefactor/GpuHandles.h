#ifndef GPU_HANDLES_H
#define GPU_HANDLES_H

#include <cstdint>

namespace Poke
{
    using ResourceHandle = uint32_t;

    inline constexpr ResourceHandle InvalidResource = 0xffffffff;

    inline bool IsValid(ResourceHandle index)
    {
        return index != InvalidResource;
    }

    struct BufferHandle
    {
        ResourceHandle index = InvalidResource;
    };

    struct TextureHandle
    {
        ResourceHandle index = InvalidResource;
    };

    struct ShaderHandle
    {
        ResourceHandle index = InvalidResource;
    };

    struct SamplerHandle
    {
        ResourceHandle index = InvalidResource;
    };

    struct DescriptorSetHandle
    {
        ResourceHandle index = InvalidResource;
    };

    struct DescriptorSetLayoutHandle
    {
        ResourceHandle index = InvalidResource;
    };

    struct PipelineHandle
    {
        ResourceHandle index = InvalidResource;
    };

    struct RenderPassHandle
    {
        ResourceHandle index = InvalidResource;
    };

    struct ShaderStateHandle
    {
        ResourceHandle index = InvalidResource;
    };

    inline bool IsValid(BufferHandle handle) { return IsValid(handle.index); }
    inline bool IsValid(TextureHandle handle) { return IsValid(handle.index); }
    inline bool IsValid(ShaderHandle handle) { return IsValid(handle.index); }
    inline bool IsValid(SamplerHandle handle) { return IsValid(handle.index); }
    inline bool IsValid(DescriptorSetHandle handle) { return IsValid(handle.index); }
    inline bool IsValid(DescriptorSetLayoutHandle handle) { return IsValid(handle.index); }
    inline bool IsValid(PipelineHandle handle) { return IsValid(handle.index); }
    inline bool IsValid(RenderPassHandle handle) { return IsValid(handle.index); }
    inline bool IsValid(ShaderStateHandle handle) { return IsValid(handle.index); }
}

#endif