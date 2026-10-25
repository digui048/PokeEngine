#ifndef GPU_HANDLES_H
#define GPU_HANDLES_H

#include <cstdint>

namespace Poke
{
    using ResourceHandle = uint32_t;

    static constexpr uint32_t k_invalidResourceIndex = 0xffffffff;
    inline constexpr ResourceHandle InvalidResource = 0xffffffff;
    static constexpr ResourceHandle k_invalidIndex = InvalidResource;

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

    static BufferHandle k_invalidBuffer{k_invalidIndex};
    static TextureHandle k_invalidTexture{k_invalidIndex};
    static ShaderStateHandle k_invalidShader{k_invalidIndex};
    static SamplerHandle k_invalidSampler{k_invalidIndex};
    static DescriptorSetLayoutHandle k_invalidLayout{k_invalidIndex};
    static DescriptorSetHandle k_invalidSet{k_invalidIndex};
    static PipelineHandle k_invalidPipeline{k_invalidIndex};
    static RenderPassHandle k_invalidPass{k_invalidIndex};

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