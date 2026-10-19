#ifndef GPU_LIMITS_H
#define GPU_LIMITS_H

#include <cstdint>

namespace Poke
{
    constexpr uint32_t kMaxImageOutputs = 8;
    constexpr uint32_t kMaxDescriptorSetLayouts = 8;
    constexpr uint32_t kMaxShaderStages = 5;
    constexpr uint32_t kMaxDescriptorPerSet = 16;
    constexpr uint32_t kMaxVertexStreams = 16;
    constexpr uint32_t kMaxVertexAttributes = 16;
    constexpr uint32_t kMaxSwapchainImages = 3;
}

#endif