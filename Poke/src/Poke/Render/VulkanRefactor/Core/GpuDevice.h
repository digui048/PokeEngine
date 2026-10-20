#ifndef GPU_DEVICE_H
#define GPU_DEVICE_H

#include "GpuResourcePool.h"

#include "GpuCreation/BufferCreation.h"
#include "GpuCreation/TextureCreation.h"
#include "GpuCreation/SamplerCreation.h"
#include "GpuCreation/ShaderCreation.h"
#include "GpuCreation/DescriptorSetCreation.h"
#include "GpuCreation/RenderPassCreation.h"
#include "GpuCreation/PipelineCreation.h"

#include "GpuResources/Buffer.h"
#include "GpuResources/Texture.h"
#include "GpuResources/Sampler.h"
#include "GpuResources/Shader.h"
#include "GpuResources/DescriptorSet.h"
#include "GpuResources/Pipeline.h"
#include "GpuResources/RenderPass.h"

#include <vector>

namespace Poke
{
    class CommandBuffer;

    enum class ResourceKey : uint8_t
    {
        Buffer,
        Texture,
        Pipeline,
        Sampler,
        DescriptorSetLayout,
        DescriptorSet,
        RenderPass,
        ShaderState
    };

    struct DeferredDeletion
    {
        ResourceKey type;
        uint32_t resourceIndex;
        uint32_t frameIndex;
    };

    struct DeviceCreation
    {
        void *window = nullptr;
        uint16_t width = 1;
        uint16_t height = 1;

        bool debug = false;

        DeviceCreation &SetWindow(uint32_t width, uint32_t height, void *handle)
        {
            this->window = handle;
            this->width = width;
            this->height = height;
            return *this;
        }

        DeviceCreation &SetDebug(bool debug)
        {
            this->debug = debug;
            return *this;
        }
    };

    class GpuDevice
    {
    public:
        static GpuDevice *GetInstance();

        GpuDevice() = default;
        ~GpuDevice();

        GpuDevice(const GpuDevice &) = delete;
        GpuDevice &operator=(const GpuDevice &) = delete;

        void Init(const DeviceCreation &creation);
        void Shutdown();

        // Create resources
        BufferHandle CreateBuffer(const BufferCreation &creation);
        TextureHandle CreateTexture(const TextureCreation &creation);
        PipelineHandle CreatePipeline(const PipelineCreation &creation);
        SamplerHandle CreateSampler(const SamplerCreation &creation);
        DescriptorSetLayoutHandle CreateDescriptorSetLayout(const DescriptorSetLayoutCreation &creation);
        DescriptorSetHandle CreateDescriptorSet(const DescriptorSetCreation &creation);
        RenderPassHandle CreateRenderPass(const RenderPassCreation &creation);
        ShaderStateHandle CreateShaderState(const ShaderStateCreation &creation);

        // Destroy resources
        void DestroyBuffer(BufferHandle buffer);
        void DestroyTexture(TextureHandle texture);
        void DestroyPipeline(PipelineHandle pipeline);
        void DestroySampler(SamplerHandle sampler);
        void DestroyDescriptorSetLayout(DescriptorSetLayoutHandle setLayout);
        void DestroyDescriptorSet(DescriptorSetHandle set);
        void DestroyRenderPass(RenderPassHandle renderPass);
        void DestroyShaderState(ShaderStateHandle shaderState);

        // Access resources
        ShaderState *AccessShaderState(ShaderStateHandle handle);
        const ShaderState *AccessShaderState(ShaderStateHandle handle) const;

        Texture *AccessTexture(TextureHandle handle);
        const Texture *AccessTexture(TextureHandle handle) const;

        Buffer *AccessBuffer(BufferHandle handle);
        const Buffer *AccessBuffer(BufferHandle handle) const;

        Pipeline *AccessPipeline(PipelineHandle handle);
        const Pipeline *AccessPipeline(PipelineHandle handle) const;

        Sampler *AccessSampler(SamplerHandle handle);
        const Sampler *AccessSampler(SamplerHandle handle) const;

        DescriptorSetLayout *AccessDescriptorSetLayout(DescriptorSetLayoutHandle handle);
        const DescriptorSetLayout *AccessDescriptorSetLayout(DescriptorSetLayoutHandle handle) const;

        DescriptorSet *AccessDescriptorSet(DescriptorSetHandle handle);
        const DescriptorSet *AccessDescriptorSet(DescriptorSetHandle handle) const;

        RenderPass *AccessRenderPass(RenderPassHandle handle);
        const RenderPass *AccessRenderPass(RenderPassHandle handle) const;

        // Describe resources
        void QueryBuffer(BufferHandle handle, BufferDescription &description) const;
        void QueryTexture(TextureHandle handle, TextureDescription &description) const;
        void QuerySampler(SamplerHandle handle, SamplerDescription &description) const;
        void QueryShaderState(ShaderStateHandle handle, ShaderStateDescription &description) const;
        void QueryDescriptorSetLayout(DescriptorSetLayoutHandle handle, DescriptorSetLayoutDescription &description) const;
        void QueryDescriptorSet(DescriptorSetHandle handle, DescriptorSetDescription &description) const;
        void QueryPipeline(PipelineHandle handle, PipelineDescription &description) const;

        // Update resources
        void UpdateDescriptorSet(DescriptorSetHandle set);
        void LinkTextureSampler(TextureHandle texture, SamplerHandle sampler);
        void ResizeOutputTextures(RenderPassHandle renderPass, uint16_t width, uint16_t height);

        // Buffer Mapping
        void *MapBuffer(BufferHandle handle);
        void UnmapBuffer(BufferHandle handle);

        // Rendering
        void NewFrame();
        void Present();

        void Resize(uint32_t width, uint32_t height);
        void SetPresentMode(PresentMode::Enum mode);

        // Command buffers
        CommandBuffer *GetCommandBuffer(QueueType::Enum type, bool begin);
        CommandBuffer *GetInstantCommandBuffer();
        void QueueCommandBuffer(CommandBuffer *commandBuffer);

        // Debugging
        void SetResourceName(VkObjectType objectType, uint64_t objectHandle, const char *name);

        // Vulkan access
        VkDevice GetVulkanDevice() const { return m_vulkanDevice; };
        VkPhysicalDevice GetVulkanPhysicalDevice() const { return m_vulkanPhysicalDevice; };
        VkDevice GetVulkanDevice() const { return m_vulkanDevice; };
        VkQueue GetVulkanQueue() const { return m_vulkanQueue; };
        uint32_t GetVulkanQueueFamily() const { return m_vulkanQueueFamily; };

    private:
        // Deferred destruction
        void QueueDeletion(ResourceKey type, uint32_t resourceIndex);
        void ProcessDeferredDeletions(uint32_t frameIndex);
        
        // Immediate destruction
        void DestroyAllResources();
        void DestroyBufferImmediate(uint32_t index);
        void DestroyTextureImmediate(uint32_t index);
        void DestroySamplerImmediate(uint32_t index);
        void DestroyShaderStateImmediate(uint32_t index);
        void DestroyDescriptorSetLayoutImmediate(uint32_t index);
        void DestroyDescriptorSetImmediate(uint32_t index);
        void DestroyPipelineImmediate(uint32_t index);
        void DestroyRenderPassImmediate(uint32_t index);

        // Vulkan initialization
        bool FindQueueFamily(VkPhysicalDevice device);
        void CreateInstance();
        void CreateSurface();
        void SelectPhysicalDevice();
        void CreateLogicalDevice();
        void CreateDescriptorPool();
        void CreateSynchronizationObjects();
        void DestroySynchronizationObjects();

        // Swapchain helpers
        void CreateSwapchain();
        void DestroySwapchain();
        void ResizeSwapchain();
        VkRenderPass GetVulkanRenderPass(const RenderPassOutput& output, const char* name);

        // Command buffer helpers
        void CreateCommandPools();
        void DestroyCommandPools();
        void ProcessDescriptorSetUpdates();

    private:
        // Device state
        bool m_initialized = false;
        void *m_windowHandle = nullptr;

        // Resource pools
        ResourcePool m_buffers;
        ResourcePool m_textures;
        ResourcePool m_pipelines;
        ResourcePool m_samplers;
        ResourcePool m_descriptorSetLayouts;
        ResourcePool m_descriptorSets;
        ResourcePool m_renderPasses;
        ResourcePool m_commandBuffers;
        ResourcePool m_shaders;

        // Internal resources
        BufferHandle m_fullscreenVertexBuffer;
        RenderPassHandle m_swapchainPass;
        SamplerHandle m_defaultSampler;
        TextureHandle m_defaultTexture;
        BufferHandle m_defaultConstantBuffer;
        TextureHandle m_depthTexture;
        RenderPassOutput m_swapchainOutput;

        // Deferred updates and destruction
        std::vector<DeferredDeletion> m_deferredDeletions;
        std::vector<DescriptorSetHandle> m_pendingDescriptorSetUpdates;
        std::vector<CommandBuffer *> m_queuedCommandBuffers;

        // Frame management
        static constexpr uint32_t kMaxFramesInFlight = 3;
        uint32_t m_currentFrame = 0;
        uint32_t m_previousFrame = 0;
        uint32_t m_vulkanImageIndex = 0;
        bool m_resized = false;
        PresentMode::Enum m_presentMode = PresentMode::VSync;

        // Swapchain dimensions
        uint32_t m_swapchainWidth = 1;
        uint32_t m_swapchainHeight = 1;
        uint32_t m_vulkanSwapchainImageCount = 0;

        // Vulkan handles
        VkInstance m_vulkanInstance;
        VkPhysicalDevice m_vulkanPhysicalDevice;
        VkPhysicalDeviceProperties m_vulkanPhysicalDeviceProperties;
        VkDevice m_vulkanDevice;
        VkQueue m_vulkanQueue;
        uint32_t m_vulkanQueueFamily;
        VkDescriptorPool m_vulkanDescriptorPool;

        // Memory allocator
        VmaAllocator m_vmaAllocator;

        // Swapchain
        VkSurfaceKHR m_vulkanWindowSurface;
        VkSurfaceFormatKHR m_vulkanSurfaceFormat;
        VkPresentModeKHR m_vulkanPresentMode;
        VkSwapchainKHR m_vulkanSwapchain;
        VkImage m_vulkanSwapchainImage[kMaxSwapchainImages];
        VkImageView m_vulkanSwapchainImageViews[kMaxSwapchainImages];
        VkFramebuffer m_vulkanSwapchainFramebuffers[kMaxSwapchainImages];

        // Command pools
        VkCommandPool m_vulkanCommandPool[kMaxFramesInFlight];
        VkCommandPool m_vulkanInstantCommandPool;

        // Synchronization
        VkSemaphore m_vulkanRenderCompleteSemaphore[kMaxSwapchainImages];
        VkSemaphore m_vulkanImageAcquiredSemaphore[kMaxFramesInFlight];
        VkFence m_vulkanCommandBufferExecutedFence[kMaxFramesInFlight];

        // Validation layers
        VkDebugUtilsMessengerEXT m_vulkanDebugMessenger;
        bool m_debugUtilsExtensionPresent = false;
    };
}

#endif