#ifndef GPU_DEVICE_H
#define GPU_DEVICE_H

#include "GpuResources.h"

namespace Poke
{
    struct DeviceCreation
    {
        void* window = nullptr;
        uint16_t width = 1;
        uint16_t height = 1;

        DeviceCreation& SetWindow(uint32_t width, uint32_t height, void* handle);
    };

    struct GpuDevice
    {
        static GpuDevice* Instance();

        void Init(const DeviceCreation& creation);
        void Shutdown();

        BufferHandle CreateBuffer(const BufferCreation& creation);
        TextureHandle CreateTexture(const TextureCreation& creation);
        PipelineHandle CreatePipeline(const PipelineCreation& creation);
        SamplerHandle CreateSampler(const SamplerCreation& creation);
        DescriptorSetLayoutHandle CreateDescriptorSetLayout(const DescriptorSetLayoutCreation& creation);
        DescriptorSetHandle CreateDescriptorSet(const DescriptorSetCreation& creation);
        RenderPassHandle CreateRenderPass(const RenderPassCreation& creation);

        void DestroyBuffer(BufferHandle buffer);
        void DestroyTexture(TextureHandle texture);
        void DestroyPipeline(PipelineHandle pipeline);
        void DestroySampler(SamplerHandle sampler);
        void DestroyDescriptorSetLayout(DescriptorSetLayoutHandle setLayout);
        void DestroyDescriptorSet(DescriptorSetHandle set);
        void DestroyRenderPass(RenderPassHandle renderPass);
    };
}

#endif