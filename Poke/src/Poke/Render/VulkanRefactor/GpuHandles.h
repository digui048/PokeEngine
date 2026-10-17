#ifndef GPU_HANDLES_H
#define GPU_HANDLES_H

#include <cstdint>

namespace Poke
{
    using ResourceHandle = uint32_t;

    ResourceHandle InvalidResource = 0xffffffff;

    struct BufferHandle {
        ResourceHandle index = InvalidResource;
    };

    struct TextureHandle {
        ResourceHandle index = InvalidResource;
    };

    struct ShaderHandle {
        ResourceHandle index = InvalidResource;
    };

    struct SamplerHandle {
        ResourceHandle index = InvalidResource;
    };

    struct DescriptorSetHandle {
        ResourceHandle index = InvalidResource;
    };

    struct DescriptorSetLayoutHandle {
        ResourceHandle index = InvalidResource;
    };

    struct PipelineHandle {
        ResourceHandle index = InvalidResource;
    };

    struct RenderPassHandle {
        ResourceHandle index = InvalidResource;
    };
}

#endif