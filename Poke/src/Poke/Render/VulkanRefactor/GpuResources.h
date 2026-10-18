#ifndef GPU_RESOURCES_H
#define GPU_RESOURCES_H

#include "GpuEnums.h"
#include "GpuHandles.h"

#include <cstdint>
#include <vulkan/vulkan.h>

namespace Poke
{
    constexpr uint32_t kMaxColorAttachments = 8;
    constexpr uint32_t kMaxDescriptorSetLayouts = 8;
    constexpr uint32_t kMaxShaderStages = 5;
    constexpr uint32_t kMaxDescriptorPerSet = 16;
    constexpr uint32_t kMaxVertexStreams = 16;
    constexpr uint32_t kMaxVertexAttributes = 16;
    constexpr uint32_t kMaxSwapchainImages = 3;

    struct Rect2DInt
    {
        int16_t x = 0;
        int16_t y = 0;
        uint16_t width = 0;
        uint16_t height = 0;
    };

    struct Viewport
    {
        Rect2DInt rect{};
        float minDepth = 0.0f;
        float maxDepth = 1.0f;
    };

    struct ViewportState
    {
        uint32_t viewportCount = 0;
        uint32_t scissorCount = 0;
        Viewport viewports[kMaxColorAttachments]{};
        Rect2DInt scissors[kMaxColorAttachments]{};
    };

    struct StencilOpState
    {
        StencilOp fail = StencilOp::Keep;
        StencilOp pass = StencilOp::Keep;
        StencilOp depthFail = StencilOp::Keep;
        CompareOp compare = CompareOp::Always;
        uint32_t compareMask = 0xff;
        uint32_t writeMask = 0xff;
        uint32_t reference = 0xff;
    };

    struct BufferCreation
    {
        BufferUsage usage = BufferUsage::None;
        ResourceUsage memoryUsage = ResourceUsage::Immutable;
        uint32_t size = 0;
        const void *initialData = nullptr;
        const char *name = nullptr;
    };

    struct TextureCreation
    {
        uint16_t width = 1;
        uint16_t height = 1;
        uint16_t depth = 1;
        uint8_t mipmaps = 1;
        Format format = Format::Undefined;
        TextureType type = TextureType::Texture2D;
        TextureUsage usage = TextureUsage::None;
        const void *initialData = nullptr;
        const char *name = nullptr;
    };

    struct SamplerCreation
    {
        Filter minFilter = Filter::Nearest;
        Filter magFilter = Filter::Nearest;
        MipmapMode mipmapMode = MipmapMode::Nearest;
        AdressMode adressU = AdressMode::Repeat;
        AdressMode adressV = AdressMode::Repeat;
        AdressMode adressW = AdressMode::Repeat;
        const char *name = nullptr;
    };

    struct ShaderStageDesc
    {
        const void *code = nullptr;
        uint32_t codeSize = 0;
        ShaderStage stage = ShaderStage::Vertex;
    };

    struct ShaderStateCreation
    {
        ShaderStageDesc stages[kMaxShaderStages]{};
        uint32_t stageCount = 0;
        const char *name = nullptr;
    };

    struct DescriptorSetLayoutBinding
    {
        DescriptorType type = DescriptorType::UniformBuffer;
        uint16_t binding = 0;
        uint16_t count = 1;
        ShaderStageFlags stageFlags = ShaderStageFlags::None;
    };

    struct DescriptorSetLayoutCreation
    {
        DescriptorSetLayoutBinding bindings[kMaxDescriptorPerSet]{};
        uint32_t bindingCount = 0;
        uint32_t setIndex = 0;
        const char *name = nullptr;
    };

    struct DescriptorSetCreation
    {
        DescriptorSetLayoutHandle layout{};
        ResourceHandle resources[kMaxDescriptorPerSet]{};
        SamplerHandle samplers[kMaxDescriptorPerSet]{};
        uint16_t bindings[kMaxDescriptorPerSet]{};
        uint32_t resourceCount = 0;
        const char *name = nullptr;
    };

    struct VertexAttribute
    {
        uint16_t location = 0;
        uint16_t binding = 0;
        uint16_t offset = 0;
        VertexFormat format = VertexFormat::Float;
    };

    struct VertexStream
    {
        uint16_t binding = 0;
        uint16_t stride = 0;
        VertexInputRate inputRate = VertexInputRate::PerVertex;
    };

    struct VertexInputCreation
    {
        VertexStream streams[kMaxVertexStreams]{};
        VertexAttribute attributes[kMaxVertexAttributes]{};
        uint32_t streamCount = 0;
        uint32_t attributeCount = 0;
    };

    struct RasterizationCreation
    {
        CullMode cullMode = CullMode::None;
        FrontFace frontFace = FrontFace::CounterClockwise;
        PolygonMode polygonMode = PolygonMode::Fill;
    };

    struct DepthStencilCreation
    {
        bool depthTest = false;
        bool depthWrite = false;
        CompareOp depthCompare = CompareOp::Always;
        bool stencilTest = false;
        StencilOpState front{};
        StencilOpState back{};
    };

    struct BlendAttachment
    {
        bool blendEnable = false;
        BlendFactor srcColor = BlendFactor::One;
        BlendFactor dstColor = BlendFactor::Zero;
        BlendOp colorOp = BlendOp::Add;
        BlendFactor srcAlpha = BlendFactor::One;
        BlendFactor dstAlpha = BlendFactor::Zero;
        BlendOp alphaOp = BlendOp::Add;
        ColorWriteMask colorWrite = ColorWriteMask::All;
    };

    struct BlendStateCreation
    {
        BlendAttachment attachments[kMaxColorAttachments]{};
        uint32_t attachmentCount = 0;
    };

    struct RenderPassOutput
    {
        Format colorFormats[kMaxColorAttachments]{};
        uint32_t colorCount = 0;
        Format depthStencilFormat = Format::Undefined;
        LoadOp colorLoad = LoadOp::DontCare;
        LoadOp depthLoad = LoadOp::DontCare;
        LoadOp stencilLoad = LoadOp::DontCare;
        StoreOp colorStore = StoreOp::DontCare;
        StoreOp depthStore = StoreOp::DontCare;
        StoreOp stencilStore = StoreOp::DontCare;
    };

    struct RenderPassCreation
    {
        RenderPassType type = RenderPassType::Geometry;
        TextureHandle colorTextures[kMaxColorAttachments]{};
        uint32_t colorCount = 0;
        TextureHandle depthTexture{};
        LoadOp colorLoad = LoadOp::DontCare;
        LoadOp depthLoad = LoadOp::DontCare;
        LoadOp stencilLoad = LoadOp::DontCare;
        StoreOp colorStore = StoreOp::DontCare;
        StoreOp depthStore = StoreOp::DontCare;
        StoreOp stencilStore = StoreOp::DontCare;
        const char *name = nullptr;
    };

    struct PipelineCreation
    {
        RasterizationCreation rasterization{};
        DepthStencilCreation depthStencil{};
        BlendStateCreation blendState{};
        VertexInputCreation vertexInput{};
        ShaderStateCreation shaders{};
        RenderPassOutput renderPass{};
        DescriptorSetLayoutHandle layouts[kMaxDescriptorSetLayouts]{};
        uint32_t layoutCount = 0;
        PrimitiveTopology topology = PrimitiveTopology::TriangleList;
        const char *name = nullptr;
    };

    struct GpuBuffer
    {
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkDeviceSize size = 0;
        BufferUsage usage = BufferUsage::None;
        ResourceUsage memoryUsage = ResourceUsage::Immutable;
        BufferHandle handle{};
        const char *name = nullptr;
    };

    struct GpuTexture
    {
        VkImage image = VK_NULL_HANDLE;
        VkImageView view = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
        uint16_t width = 1;
        uint16_t height = 1;
        uint16_t depth = 1;
        uint8_t mipmaps = 1;
        Format format = Format::Undefined;
        TextureType type = TextureType::Texture2D;
        TextureUsage usage = TextureUsage::None;
        TextureHandle handle{};
        const char *name = nullptr;
    };

    struct GpuSampler
    {
        VkSampler sampler = VK_NULL_HANDLE;
        Filter minFilter = Filter::Nearest;
        Filter magFilter = Filter::Nearest;
        MipmapMode mipmapMode = MipmapMode::Nearest;
        AdressMode adressU = AdressMode::Repeat;
        AdressMode adressV = AdressMode::Repeat;
        AdressMode adressW = AdressMode::Repeat;
        const char *name = nullptr;
    };

    struct GpuShaderState
    {
        VkShaderModule modules[kMaxShaderStages]{};
        VkPipelineShaderStageCreateInfo stageInfos[kMaxShaderStages]{};
        uint32_t stageCount = 0;
        bool isGraphicsPipeline = true;
        const char *name = nullptr;
    };

    struct GpuDescriptorSetLayout
    {
        VkDescriptorSetLayout layout = VK_NULL_HANDLE;
        DescriptorSetLayoutBinding bindings[kMaxDescriptorPerSet]{};
        uint32_t bindingCount = 0;
        DescriptorSetLayoutHandle handle{};
    };

    struct GpuDescriptorSet
    {
        VkDescriptorSet set = VK_NULL_HANDLE;
        ResourceHandle resources[kMaxDescriptorPerSet]{};
        SamplerHandle samplers[kMaxDescriptorPerSet]{};
        uint16_t bindings[kMaxDescriptorPerSet]{};
        uint32_t resourceCount = 0;
        DescriptorSetLayoutHandle layout{};
    };

    struct GpuPipeline
    {
        VkPipeline pipeline = VK_NULL_HANDLE;
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
        VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        ShaderHandle shader{};
        DescriptorSetLayoutHandle layouts[kMaxDescriptorSetLayouts]{};
        uint32_t layoutCount = 0;
        PipelineHandle handle{};
        bool isGraphics = true;
    };

    struct GpuRenderPass
    {
        VkRenderPass renderPass = VK_NULL_HANDLE;
        VkFramebuffer framebuffer = VK_NULL_HANDLE;
        RenderPassOutput output{};
        TextureHandle colorTextures[kMaxColorAttachments];
        TextureHandle depthTexture{};
        uint32_t colorCount = 0;
        uint16_t width = 0;
        uint16_t height = 0;
        RenderPassType type = RenderPassType::Geometry;
        const char * name = nullptr;
    };
}

#endif