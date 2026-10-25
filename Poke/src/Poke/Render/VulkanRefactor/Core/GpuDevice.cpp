#include "GpuDevice.h"
#include "Poke/Core/Log.h"
#include "Poke/Core/Assert.h"

#include "../Command/CommandBufferRing.h"

#include <SDL3/SDL_vulkan.h>
#include <cstring>

using namespace Poke;

static const char *s_requestedLayers[] = {"VK_LAYER_KHRONOS_validation"};
static const char *s_requestedExtensions[] = {VK_KHR_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_REPORT_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME};

static SDL_Window *s_sdlWindow;

PFN_vkSetDebugUtilsObjectNameEXT pfnSetDebugUtilsObjectNameEXT;
PFN_vkCmdBeginDebugUtilsLabelEXT pfnCmdBeginDebugUtilsLabelEXT;
PFN_vkCmdEndDebugUtilsLabelEXT pfnCmdEndDebugUtilsLabelEXT;

static std::unordered_map<uint64_t, VkRenderPass> s_renderPassCache;
static CommandBufferRing s_commandBufferRing;

static size_t s_uboAlignment = 256;
static size_t s_ssboAlignment = 256;

static bool IsValidationsEnabled()
{
#ifdef NDEBUG
    return false;
#else
    return true;
#endif
}

static VkBool32 debugUtilsCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT types, const VkDebugUtilsMessengerCallbackDataEXT *callbackData, void *userData)
{
    if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        POKE_CORE_ERROR("[VulkanValidation] {0}", callbackData->pMessage);
    else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        POKE_CORE_WARN("[VulkanValidation] {0}", callbackData->pMessage);
    else
        POKE_CORE_INFO("[VulkanValidation] {0}", callbackData->pMessage);

    return VK_FALSE;
}

VkDebugUtilsMessengerCreateInfoEXT createDebugUtilsMessengerInfo()
{
    VkDebugUtilsMessengerCreateInfoEXT creationInfo = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
    creationInfo.pfnUserCallback = debugUtilsCallback;
    creationInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    creationInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;

    return creationInfo;
}

bool GpuDevice::FindQueueFamily(VkPhysicalDevice device)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_vulkanPhysicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_vulkanPhysicalDevice, &queueFamilyCount, queueFamilies.data());

    uint32_t familyIndex;
    VkBool32 surfaceSupported;
    for (familyIndex = 0; familyIndex < queueFamilyCount; ++familyIndex)
    {
        VkQueueFamilyProperties queueFamily = queueFamilies[familyIndex];
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT))
        {
            vkGetPhysicalDeviceSurfaceSupportKHR(m_vulkanPhysicalDevice, familyIndex, m_vulkanWindowSurface, &surfaceSupported);
            if (surfaceSupported)
            {
                m_vulkanQueueFamily = familyIndex;
                break;
            }
        }
    }

    return surfaceSupported;
}

void GpuDevice::CreateInstance(const DeviceCreation &creation)
{
    VkResult result;
    VkApplicationInfo applicationInfo = {VK_STRUCTURE_TYPE_APPLICATION_INFO, nullptr, "Poke Graphics Device", 1, "PokeEngine", 1, VK_MAKE_VERSION(1, 2, 0)};
    VkInstanceCreateInfo createInfo;
    if (IsValidationsEnabled())
        createInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, nullptr, 0, &applicationInfo, sizeof(s_requestedLayers) / sizeof((s_requestedLayers)[0]), s_requestedLayers, sizeof(s_requestedExtensions) / sizeof((s_requestedExtensions)[0]), s_requestedExtensions};
    else
        createInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, nullptr, 0, &applicationInfo, 0, nullptr, 0, nullptr};
    // Create debug validation layers
    const VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = createDebugUtilsMessengerInfo();
    createInfo.pNext = &debugCreateInfo;
    result = vkCreateInstance(&createInfo, m_vulkanAllocationCallbacks, &m_vulkanInstance);
    POKE_ASSERT(result == VK_SUCCESS, "Failed to Create vulkan instance");

    m_swapchainWidth = creation.width;
    m_swapchainHeight = creation.height;

    uint32_t numInstanceExtensions;
    vkEnumerateInstanceExtensionProperties(nullptr, &numInstanceExtensions, nullptr);
    std::vector<VkExtensionProperties> extensions(numInstanceExtensions);
    vkEnumerateInstanceExtensionProperties(nullptr, &numInstanceExtensions, extensions.data());
    for (size_t i = 0; i < numInstanceExtensions; ++i)
    {
        if (!std::strcmp(extensions[i].extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
        {
            m_debugUtilsExtensionPresent = true;
            break;
        }
    }
    if (!m_debugUtilsExtensionPresent)
        POKE_CORE_INFO("[GpuDevice] Extension {0} for debugging not present", VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    else
    {
        PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_vulkanInstance, "vkCreateDebugUtilsMessengerEXT");
        VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = createDebugUtilsMessengerInfo();

        vkCreateDebugUtilsMessengerEXT(m_vulkanInstance, &debugMessengerCreateInfo, m_vulkanAllocationCallbacks, &m_vulkanDebugMessenger);
    }
}

void GpuDevice::CreateSurface(const DeviceCreation &creation)
{
    SDL_Window *window = (SDL_Window *)creation.window;
    if (!SDL_Vulkan_CreateSurface(window, m_vulkanInstance, nullptr, &m_vulkanWindowSurface))
        POKE_CORE_ERROR("[GpuDevice] Failed to create Vulkan surface");

    s_sdlWindow = window;
}

void GpuDevice::SelectPhysicalDevice()
{
    VkResult result;
    uint32_t numPhysicalDevice;
    result = vkEnumeratePhysicalDevices(m_vulkanInstance, &numPhysicalDevice, NULL);
    POKE_ASSERT(result == VK_SUCCESS, "Failed to enumerate physical devices");
    std::vector<VkPhysicalDevice> gpus(numPhysicalDevice);
    result = vkEnumeratePhysicalDevices(m_vulkanInstance, &numPhysicalDevice, gpus.data());
    POKE_ASSERT(result == VK_SUCCESS, "Failed to enumerate physical devices");

    VkPhysicalDevice discreteGpu = VK_NULL_HANDLE;
    VkPhysicalDevice integratedGpu = VK_NULL_HANDLE;
    for (uint32_t i = 0; i < numPhysicalDevice; ++i)
    {
        VkPhysicalDevice physicalDevice = gpus[i];
        vkGetPhysicalDeviceProperties(physicalDevice, &m_vulkanPhysicalDeviceProperties);

        if (m_vulkanPhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            if (FindQueueFamily(m_vulkanPhysicalDevice))
            {
                discreteGpu = physicalDevice;
                break;
            }
            continue;
        }

        if (m_vulkanPhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        {
            if (FindQueueFamily(m_vulkanPhysicalDevice))
                integratedGpu = physicalDevice;

            continue;
        }
    }

    if (discreteGpu != VK_NULL_HANDLE)
        m_vulkanPhysicalDevice = discreteGpu;
    else if (integratedGpu != VK_NULL_HANDLE)
        m_vulkanPhysicalDevice = integratedGpu;
    else
    {
        POKE_ASSERT(true, "[GpuDevice] Suitable Gpu Device not found");
        return;
    }

    vkGetPhysicalDeviceProperties(m_vulkanPhysicalDevice, &m_vulkanPhysicalDeviceProperties);

    s_uboAlignment = m_vulkanPhysicalDeviceProperties.limits.minUniformBufferOffsetAlignment;
    s_ssboAlignment = m_vulkanPhysicalDeviceProperties.limits.minStorageBufferOffsetAlignment;
}

void GpuDevice::CreateLogicalDevice()
{
    uint32_t deviceExtensionCount = 1;
    const char *deviceExtensions[] = {"VK_KHR_swapchain"};
    const float queuePriority[] = {1.0f};
    VkDeviceQueueCreateInfo queueInfo[1] = {};
    queueInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo[0].queueFamilyIndex = m_vulkanQueueFamily;
    queueInfo[0].queueCount = 1;
    queueInfo[0].pQueuePriorities = queuePriority;

    VkPhysicalDeviceFeatures2 physicalFeatures2 = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
    vkGetPhysicalDeviceFeatures2(m_vulkanPhysicalDevice, &physicalFeatures2);

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = sizeof(queueInfo) / sizeof(queueInfo[0]);
    deviceCreateInfo.pQueueCreateInfos = queueInfo;
    deviceCreateInfo.enabledExtensionCount = deviceExtensionCount;
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions;
    deviceCreateInfo.pNext = &physicalFeatures2;

    VkResult result = vkCreateDevice(m_vulkanPhysicalDevice, &deviceCreateInfo, m_vulkanAllocationCallbacks, &m_vulkanDevice);
    POKE_ASSERT(result == VK_SUCCESS, "Failed to create logical device");

    if (m_debugUtilsExtensionPresent)
    {
        pfnSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetDeviceProcAddr(m_vulkanDevice, "vkSetDebugUtilsObjectNameEXT");
        pfnCmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetDeviceProcAddr(m_vulkanDevice, "vkCmdBeginDebugUtilsLabelEXT");
        pfnCmdEndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetDeviceProcAddr(m_vulkanDevice, "vkCmdEndDebugUtilsLabelEXT");
    }

    vkGetDeviceQueue(m_vulkanDevice, m_vulkanQueueFamily, 0, &m_vulkanQueue);

    int windowW, windowH;
    SDL_GetWindowSize(s_sdlWindow, &windowW, &windowH);

    const VkFormat surfaceImageFormats[] = {VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM};
    const VkColorSpaceKHR surfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

    uint32_t supportedCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_vulkanPhysicalDevice, m_vulkanWindowSurface, &supportedCount, NULL);
    std::vector<VkSurfaceFormatKHR> supportedFormats(supportedCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_vulkanPhysicalDevice, m_vulkanWindowSurface, &supportedCount, supportedFormats.data());

    m_swapchainOutput.Reset();

    bool formatFound = false;
    const uint32_t surfaceFormatCount = sizeof(surfaceImageFormats) / sizeof((surfaceImageFormats)[0]);
    for (int i = 0; i < surfaceFormatCount; ++i)
    {
        for (uint32_t j = 0; j < supportedCount; ++j)
        {
            if (supportedFormats[j].format == surfaceImageFormats[i] && supportedFormats[j].colorSpace == surfaceColorSpace)
            {
                m_vulkanSurfaceFormat = supportedFormats[j];
                formatFound = true;
                break;
            }
        }

        if (formatFound)
            break;
    }

    if (!formatFound)
    {
        m_vulkanSurfaceFormat = supportedFormats[0];
        POKE_ASSERT(false, "Failed to find supported formats");
    }

    m_swapchainOutput.SetColor(m_vulkanSurfaceFormat.format);
    SetPresentMode(m_presentMode);

    // Create swapchain and VMA allocator
    CreateSwapchain();

    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = m_vulkanPhysicalDevice;
    allocatorInfo.device = m_vulkanDevice;
    allocatorInfo.instance = m_vulkanInstance;

    result = vmaCreateAllocator(&allocatorInfo, &m_vmaAllocator);
    POKE_ASSERT(result == VK_SUCCESS, "Failed to create VMA allocator");
}

void GpuDevice::CreateDescriptorPool()
{
    static const uint32_t k_globalPoolElements = 128;
    VkDescriptorPoolSize poolSizes[] =
        {
            {VK_DESCRIPTOR_TYPE_SAMPLER, k_globalPoolElements},
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, k_globalPoolElements},
            {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, k_globalPoolElements},
            {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, k_globalPoolElements},
            {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, k_globalPoolElements},
            {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, k_globalPoolElements},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, k_globalPoolElements},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, k_globalPoolElements},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, k_globalPoolElements},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, k_globalPoolElements},
            {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, k_globalPoolElements}};

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.maxSets = k_globalPoolElements * (sizeof(poolSizes) / sizeof((poolSizes)[0]));
    poolInfo.poolSizeCount = static_cast<uint32_t>(sizeof(poolSizes) / sizeof((poolSizes)[0]));
    poolInfo.pPoolSizes = poolSizes;
    VkResult result = vkCreateDescriptorPool(m_vulkanDevice, &poolInfo, m_vulkanAllocationCallbacks, &m_vulkanDescriptorPool);
    POKE_ASSERT(result == VK_SUCCESS, "Failed to create descriptor pool");

    m_buffers.Init(4096, sizeof(Buffer));
    m_textures.Init(512, sizeof(Texture));
    m_renderPasses.Init(256, sizeof(RenderPass));
    m_descriptorSetLayouts.Init(128, sizeof(DescriptorSetLayout));
    m_pipelines.Init(128, sizeof(Pipeline));
    m_shaders.Init(128, sizeof(ShaderState));
    m_descriptorSets.Init(128, sizeof(DescriptorSet));
    m_samplers.Init(32, sizeof(Sampler));
}

void GpuDevice::CreateSynchronizationObjects()
{
    VkSemaphoreCreateInfo semaphoreInfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    for (size_t i = 0; i < kMaxSwapchainImages; ++i)
    {
        vkCreateSemaphore(m_vulkanDevice, &semaphoreInfo, m_vulkanAllocationCallbacks, &m_vulkanRenderCompleteSemaphore[i]);
        vkCreateSemaphore(m_vulkanDevice, &semaphoreInfo, m_vulkanAllocationCallbacks, &m_vulkanImageAcquiredSemaphore[i]);

        VkFenceCreateInfo fenceInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        vkCreateFence(m_vulkanDevice, &fenceInfo, m_vulkanAllocationCallbacks, &m_vulkanCommandBufferExecutedFence[i]);
    }

    m_vulkanImageIndex = 0;
    m_currentFrame = 1;
    m_previousFrame = 0;
    m_absoluteFrame = 0;
}

void GpuDevice::Init(const DeviceCreation &creation)
{
    POKE_CORE_INFO("[GpuDevice] Gpu Device initializing");

    CreateInstance(creation);
    CreateSurface(creation);
    SelectPhysicalDevice();
    CreateLogicalDevice();
    CreateDescriptorPool();
    CreateSynchronizationObjects();

    s_commandBufferRing.Init(this);
    m_queuedCommandBuffers.reserve(128);

    m_resourceDeletionQueue.reserve(16);
    m_descriptorSetUpdates.reserve(16);

    // Create default sampler
    SamplerCreation samplerCreation;
    samplerCreation.SetAddressModeUVW(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)
        .SetMinMagMip(VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR)
        .SetName("SamplerDefault");
    m_defaultSampler = CreateSampler(samplerCreation);

    // Create fullscreen vertex buffer
    BufferCreation bufferCreation = {VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, ResourceUsageType::Immutable, 0, nullptr, "FullscreenVertexBuffer"};
    m_fullscreenVertexBuffer = CreateBuffer(bufferCreation);

    // Create depth image
    TextureCreation depthTextureCreation = {nullptr, static_cast<uint16_t>(m_swapchainWidth), static_cast<uint16_t>(m_swapchainHeight), 1, 1, 0, VK_FORMAT_D32_SFLOAT, TextureType::Texture2D, "DepthTexture"};
    m_depthTexture = CreateTexture(depthTextureCreation);

    // Cache depth texture format
    m_swapchainOutput.SetDepthStencil(VK_FORMAT_D32_SFLOAT);

    // Create swapchain render pass
    RenderPassCreation swapchainPassCreation;
    swapchainPassCreation.SetType(RenderPassType::Swapchain).SetName("Swapchain");
    swapchainPassCreation.SetOperations(RenderPassOperation::Clear, RenderPassOperation::Clear, RenderPassOperation::Clear);
    m_swapchainPass = CreateRenderPass(swapchainPassCreation);

    // Create default texture and constant buffer
    TextureCreation defaultTextureCreation = {nullptr, 1, 1, 1, 1, 0, VK_FORMAT_R8_UINT, TextureType::Texture2D};
    m_defaultTexture = CreateTexture(defaultTextureCreation);

    BufferCreation defaultConstantBufferCreation = {VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, ResourceUsageType::Immutable, 16, nullptr, "DefaultConstantBuffer"};
    m_defaultConstantBuffer = CreateBuffer(defaultConstantBufferCreation);

    // Create dynamic buffer
    m_dynamicPerFrameSize = 1024 * 1024 * 10;
    BufferCreation dynamicBufferCreation;
    dynamicBufferCreation.Set(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, ResourceUsageType::Immutable, m_dynamicPerFrameSize * 3).SetName("DynamicBuffer");
    m_dynamicBuffer = CreateBuffer(dynamicBufferCreation);

    MapBufferParameters constantBufferMap = {m_dynamicBuffer, 0, 0};
    m_dynamicMappedMemory = static_cast<uint8_t *>(MapBuffer(constantBufferMap));
}

void GpuDevice::Shutdown()
{
    vkDeviceWaitIdle(m_vulkanDevice);

    s_commandBufferRing.Shutdown();

    for (size_t i = 0; i < kMaxSwapchainImages; ++i)
    {
        vkDestroySemaphore(m_vulkanDevice, m_vulkanRenderCompleteSemaphore[i], m_vulkanAllocationCallbacks);
        vkDestroySemaphore(m_vulkanDevice, m_vulkanImageAcquiredSemaphore[i], m_vulkanAllocationCallbacks);
        vkDestroyFence(m_vulkanDevice, m_vulkanCommandBufferExecutedFence[i], m_vulkanAllocationCallbacks);
    }

    for (uint32_t i = 0; i < static_cast<uint32_t>(m_resourceDeletionQueue.size()); ++i)
    {
        ResourceUpdate &resourceDeletion = m_resourceDeletionQueue[i];

        if (resourceDeletion.currentFrame == -1)
            continue;

        switch (resourceDeletion.type)
        {
        case ResourceDeletionType::Buffer:
        {
            DestroyBufferInstant(resourceDeletion.handle);
            break;
        }

        case ResourceDeletionType::Pipeline:
        {
            DestroyPipelineInstant(resourceDeletion.handle);
            break;
        }

        case ResourceDeletionType::RenderPass:
        {
            DestroyRenderPassInstant(resourceDeletion.handle);
            break;
        }

        case ResourceDeletionType::DescriptorSet:
        {
            DestroyDescriptorSetInstant(resourceDeletion.handle);
            break;
        }

        case ResourceDeletionType::DescriptorSetLayout:
        {
            DestroyDescriptorSetLayoutInstant(resourceDeletion.handle);
            break;
        }

        case ResourceDeletionType::Sampler:
        {
            DestroySamplerImmediate(resourceDeletion.handle);
            break;
        }

        case ResourceDeletionType::ShaderState:
        {
            DestroyShaderStateInstant(resourceDeletion.handle);
            break;
        }

        case ResourceDeletionType::Texture:
        {
            DestroyTextureInstant(resourceDeletion.handle);
            break;
        }
        }
    }

    std::unordered_map<uint64_t, VkRenderPass>::iterator it = s_renderPassCache.begin();
    while (it != s_renderPassCache.end())
    {
        VkRenderPass renderPass = it->second;
        vkDestroyRenderPass(m_vulkanDevice, renderPass, m_vulkanAllocationCallbacks);
        ++it;
    }
    s_renderPassCache.clear();

    RenderPass *vkSwapchainPass = AccessRenderPass(m_swapchainPass);
    vkDestroyRenderPass(m_vulkanDevice, vkSwapchainPass->renderPass, m_vulkanAllocationCallbacks);

    DestroySwapchain();
    vkDestroySurfaceKHR(m_vulkanInstance, m_vulkanWindowSurface, m_vulkanAllocationCallbacks);

    vmaDestroyAllocator(m_vmaAllocator);

    m_resourceDeletionQueue.clear();
    m_descriptorSetUpdates.clear();

    m_pipelines.Shutdown();
    m_buffers.Shutdown();
    m_shaders.Shutdown();
    m_textures.Shutdown();
    m_samplers.Shutdown();
    m_descriptorSetLayouts.Shutdown();
    m_descriptorSets.Shutdown();
    m_renderPasses.Shutdown();

    if (IsValidationsEnabled() && m_debugUtilsExtensionPresent)
    {
        PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_vulkanInstance, "vkDestroyDebugUtilsMessengerEXT");
        vkDestroyDebugUtilsMessengerEXT(m_vulkanInstance, m_vulkanDebugMessenger, m_vulkanAllocationCallbacks);
    }

    vkDestroyDescriptorPool(m_vulkanDevice, m_vulkanDescriptorPool, m_vulkanAllocationCallbacks);
    vkDestroyDevice(m_vulkanDevice, m_vulkanAllocationCallbacks);
    vkDestroyInstance(m_vulkanInstance, m_vulkanAllocationCallbacks);

    POKE_CORE_INFO("[GpuDevice] Gpu Device shutdown complete");
}

BufferHandle GpuDevice::CreateBuffer(const BufferCreation &creation)
{
    BufferHandle handle = {m_buffers.ObtainResource()};
    if (handle.index == k_invalidResourceIndex)
        return handle;

    Buffer *buffer = AccessBuffer(handle);
    buffer->name = creation.name;
    buffer->size = creation.size;
    buffer->typeFlags = creation.typeFlags;
    buffer->usage = creation.usage;
    buffer->handle = handle;
    buffer->globalOffset = 0;
    buffer->parentHandle = k_invalidBuffer;

    static const VkBufferUsageFlags k_dynamicBufferMask = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    const bool useGlobalBuffer = (creation.typeFlags & k_dynamicBufferMask) != 0;
    if (creation.usage == ResourceUsageType::Dynamic && useGlobalBuffer)
    {
        buffer->parentHandle = m_dynamicBuffer;
        return handle;
    }

    VkBufferCreateInfo bufferCreateInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | creation.typeFlags;
    bufferCreateInfo.size = creation.size > 0 ? creation.size : 1;

    VmaAllocationCreateInfo memoryInfo = {};
    memoryInfo.flags = VMA_ALLOCATION_CREATE_STRATEGY_BEST_FIT_BIT;
    memoryInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    VmaAllocationInfo allocationInfo{};
    VkResult result = vmaCreateBuffer(m_vmaAllocator, &bufferCreateInfo, &memoryInfo, &buffer->buffer, &buffer->allocation, &allocationInfo);
    POKE_ASSERT(result == VK_SUCCESS, "Failed to create buffer");
    SetResourceName(VK_OBJECT_TYPE_BUFFER, reinterpret_cast<uint64_t>(buffer->buffer), creation.name);

    buffer->memory = allocationInfo.deviceMemory;

    if (creation.initialData)
    {
        void *data;
        vmaMapMemory(m_vmaAllocator, buffer->allocation, &data);
        memcpy(data, creation.initialData, static_cast<size_t>(creation.size));
        vmaUnmapMemory(m_vmaAllocator, buffer->allocation);
    }

    return handle;
}

TextureHandle GpuDevice::CreateTexture(const TextureCreation &creation)
{
    uint32_t resourceIndex = m_textures.ObtainResource();
    TextureHandle handle = {resourceIndex};
    if (resourceIndex == k_invalidIndex)
        return handle;

    Texture *texture = AccessTexture(handle);
    texture->width = creation.width;
    texture->height = creation.height;
    texture->depth = creation.depth;
    texture->mipmaps = creation.mipmaps;
    texture->type = creation.type;
    texture->name = creation.name;
    texture->format = creation.format;
    texture->sampler = nullptr;
    texture->flags = creation.flags;

    texture->handle = handle;

    VkImageCreateInfo imageCreateInfo = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    imageCreateInfo.format = texture->format;
    imageCreateInfo.flags = 0;
    imageCreateInfo.imageType = /*converter(creation.type)*/ VK_IMAGE_TYPE_2D; // here should be a converter enum to vkImagetype
    imageCreateInfo.extent.width = creation.width;
    imageCreateInfo.extent.height = creation.height;
    imageCreateInfo.extent.depth = creation.depth;
    imageCreateInfo.mipLevels = creation.mipmaps;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;

    const bool isRenderTarget = (creation.flags & TextureFlags::RenderTargetMask) == TextureFlags::RenderTargetMask;
    const bool isComputeUsed = (creation.flags & TextureFlags::ComputeMask) == TextureFlags::ComputeMask;

    imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
    imageCreateInfo.usage |= isComputeUsed ? VK_IMAGE_USAGE_STORAGE_BIT : 0;

    if (TextureFormat::HasDepthOrStencil(creation.format))
    {
        imageCreateInfo.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    }
    else
    {
        imageCreateInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        imageCreateInfo.usage |= isRenderTarget ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT : 0;
    }

    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VmaAllocationCreateInfo memoryInfo{};
    memoryInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    VkResult result = vmaCreateImage(m_vmaAllocator, &imageCreateInfo, &memoryInfo, &texture->image, &texture->allocation, nullptr);
    POKE_ASSERT(result == VK_SUCCESS, "Failed to create texture image");
    SetResourceName(VK_OBJECT_TYPE_IMAGE, reinterpret_cast<uint64_t>(texture->image), creation.name);

    VkImageViewCreateInfo info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    info.image = texture->image;
    info.viewType = /*converter(creation.type)*/ VK_IMAGE_VIEW_TYPE_2D; // here should be a converter enum to vkImageViewType
    info.format = imageCreateInfo.format;

    if (TextureFormat::HasDepthOrStencil(creation.format))
    {
        info.subresourceRange.aspectMask = TextureFormat::HasDepth(creation.format) ? VK_IMAGE_ASPECT_DEPTH_BIT : 0;
    }
    else
    {
        info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    info.subresourceRange.levelCount = 1;
    info.subresourceRange.layerCount = 1;
    result = vkCreateImageView(m_vulkanDevice, &info, m_vulkanAllocationCallbacks, &texture->view);
    POKE_ASSERT(result == VK_SUCCESS, "Failed to create texture image view");

    SetResourceName(VK_OBJECT_TYPE_IMAGE_VIEW, reinterpret_cast<uint64_t>(texture->view), creation.name);

    texture->layout = VK_IMAGE_LAYOUT_UNDEFINED;

    if (creation.initialData)
    {
        VkBufferCreateInfo bufferInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        uint32_t imageSize = creation.width * creation.height * 4;
        bufferInfo.size = imageSize;

        VmaAllocationCreateInfo memoryInfo{};
        memoryInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
        memoryInfo.flags = VMA_ALLOCATION_CREATE_STRATEGY_BEST_FIT_BIT;

        VmaAllocationInfo allocationInfo{};
        VkBuffer stagingBuffer;
        VmaAllocation stagingAllocation;
        result = vmaCreateBuffer(m_vmaAllocator, &bufferInfo, &memoryInfo, &stagingBuffer, &stagingAllocation, &allocationInfo);
        POKE_ASSERT(result == VK_SUCCESS, "Failed to create staging buffer");

        void *destData;
        vmaMapMemory(m_vmaAllocator, stagingAllocation, &destData);
        memcpy(destData, creation.initialData, static_cast<size_t>(imageSize));
        vmaUnmapMemory(m_vmaAllocator, stagingAllocation);

        VkCommandBufferBeginInfo beginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        CommandBuffer *commandBuffer = GetInstantCommandBuffer();
        vkBeginCommandBuffer(commandBuffer->m_commandBuffer, &beginInfo);

        VkBufferImageCopy region = {};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {creation.width, creation.height, creation.depth};

        VkImageMemoryBarrier barrierF = {};
        barrierF.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrierF.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrierF.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

        barrierF.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrierF.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        barrierF.image = texture->image;
        barrierF.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrierF.subresourceRange.baseMipLevel = 0;
        barrierF.subresourceRange.levelCount = 1;
        barrierF.subresourceRange.baseArrayLayer = 0;
        barrierF.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStageF = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        VkPipelineStageFlags dstStageF = VK_PIPELINE_STAGE_TRANSFER_BIT;

        barrierF.srcAccessMask = 0;
        barrierF.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        vkCmdPipelineBarrier(commandBuffer->m_commandBuffer, sourceStageF, dstStageF, 0, 0, nullptr, 0, nullptr, 1, &barrierF);

        vkCmdCopyBufferToImage(commandBuffer->m_commandBuffer, stagingBuffer, texture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        VkImageMemoryBarrier barrierS = {};
        barrierS.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrierS.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrierS.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        barrierS.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrierS.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        barrierS.image = texture->image;
        barrierS.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrierS.subresourceRange.baseMipLevel = 0;
        barrierS.subresourceRange.levelCount = 1;
        barrierS.subresourceRange.baseArrayLayer = 0;
        barrierS.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStageS = VK_PIPELINE_STAGE_TRANSFER_BIT;
        VkPipelineStageFlags dstStageS = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

        barrierS.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrierS.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer->m_commandBuffer, sourceStageS, dstStageS, 0, 0, nullptr, 0, nullptr, 1, &barrierS);

        vkEndCommandBuffer(commandBuffer->m_commandBuffer);

        VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer->m_commandBuffer;

        vkQueueSubmit(m_vulkanQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(m_vulkanQueue);

        vmaDestroyBuffer(m_vmaAllocator, stagingBuffer, stagingAllocation);

        vkResetCommandBuffer(commandBuffer->m_commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

        texture->layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }

    return handle;
}

PipelineHandle GpuDevice::CreatePipeline(const PipelineCreation &creation)
{
    PipelineHandle handle = {m_pipelines.ObtainResource()};
    if (handle.index == k_invalidIndex)
        return handle;

    ShaderStateHandle shaderState = CreateShaderState(creation.shaders);
    if (shaderState.index == k_invalidIndex)
    {
        m_pipelines.ReleaseResource(handle.index);
        handle.index = k_invalidIndex;

        return handle;
    }

    Pipeline *pipeline = AccessPipeline(handle);
    ShaderState *shaderStateData = AccessShaderState(shaderState);

    pipeline->shaderState = shaderState;

    VkDescriptorSetLayout vkLayouts[kMaxDescriptorSetLayouts];

    for (uint32_t i = 0; i < creation.activeLayoutsCount; ++i)
    {
        pipeline->descriptorSetLayouts[i] = AccessDescriptorSetLayout(creation.descriptorSetLayouts[i]);
        pipeline->descriptorSetLayoutHandles[i] = creation.descriptorSetLayouts[i];

        vkLayouts[i] = pipeline->descriptorSetLayouts[i]->layout;
    }

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    pipelineLayoutInfo.pSetLayouts = vkLayouts;
    pipelineLayoutInfo.setLayoutCount = creation.activeLayoutsCount;

    VkPipelineLayout pipelineLayout;
    VkResult result = vkCreatePipelineLayout(m_vulkanDevice, &pipelineLayoutInfo, m_vulkanAllocationCallbacks, &pipelineLayout);
    POKE_ASSERT(result == VK_SUCCESS, "Failed to create Pipeline layout");
    pipeline->pipelineLayout = pipelineLayout;
    pipeline->numActiveLayouts = creation.activeLayoutsCount;

    if (shaderStateData->isGraphicsPipeline)
    {
        VkGraphicsPipelineCreateInfo pipelineInfo = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
        pipelineInfo.pStages = shaderStateData->stageInfos;
        pipelineInfo.stageCount = shaderStateData->activeShaders;
        pipelineInfo.layout = pipelineLayout;

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
        VkVertexInputAttributeDescription vertexAttributes[8];
        if (creation.vertexInput.attributeCount)
        {
            for (uint32_t i = 0; i < creation.vertexInput.attributeCount; ++i)
            {
                const VertexAttribute &vertexAttribute = creation.vertexInput.attributes[i];
                vertexAttributes[i] = {vertexAttribute.location, vertexAttribute.binding, /*converter(vertexAttribute.format)*/ VK_FORMAT_R8_SNORM, vertexAttribute.offset}; // here it should be a converter
            }

            vertexInputInfo.vertexAttributeDescriptionCount = creation.vertexInput.attributeCount;
            vertexInputInfo.pVertexAttributeDescriptions = vertexAttributes;
        }
        else
        {
            vertexInputInfo.vertexAttributeDescriptionCount = 0;
            vertexInputInfo.pVertexAttributeDescriptions = nullptr;
        }

        VkVertexInputBindingDescription vertexBindings[8];
        if (creation.vertexInput.streamCount)
        {
            vertexInputInfo.vertexBindingDescriptionCount = creation.vertexInput.streamCount;

            for (uint32_t i = 0; i < creation.vertexInput.streamCount; ++i)
            {
                const VertexStream &vertexStream = creation.vertexInput.streams[i];
                VkVertexInputRate vertexRate = vertexStream.inputRate == VertexInputRate::PerVertex ? VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX : VkVertexInputRate::VK_VERTEX_INPUT_RATE_INSTANCE;
                vertexBindings[i] = {vertexStream.binding, vertexStream.stride, vertexRate};
            }
            vertexInputInfo.pVertexBindingDescriptions = vertexBindings;
        }
        else
        {
            vertexInputInfo.vertexBindingDescriptionCount = 0;
            vertexInputInfo.pVertexBindingDescriptions = nullptr;
        }

        pipelineInfo.pVertexInputState = &vertexInputInfo;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        pipelineInfo.pInputAssemblyState = &inputAssembly;

        VkPipelineColorBlendAttachmentState colorBlendAttachment[8];
        if (creation.blendState.activeStates)
        {
            for (size_t i = 0; i < creation.blendState.activeStates; ++i)
            {
                const BlendState &blendState = creation.blendState.blendStates[i];
                colorBlendAttachment[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
                colorBlendAttachment[i].blendEnable = blendState.blendEnable ? VK_TRUE : VK_FALSE;
                colorBlendAttachment[i].srcColorBlendFactor = blendState.sourceColor;
                colorBlendAttachment[i].dstColorBlendFactor = blendState.destinationColor;
                colorBlendAttachment[i].colorBlendOp = blendState.colorOperation;

                if (blendState.separeteBlend)
                {
                    colorBlendAttachment[i].srcAlphaBlendFactor = blendState.sourceAlpha;
                    colorBlendAttachment[i].dstAlphaBlendFactor = blendState.destinationAlpha;
                    colorBlendAttachment[i].alphaBlendOp = blendState.alphaOperation;
                }
                else
                {
                    colorBlendAttachment[i].srcAlphaBlendFactor = blendState.sourceColor;
                    colorBlendAttachment[i].dstAlphaBlendFactor = blendState.destinationColor;
                    colorBlendAttachment[i].alphaBlendOp = blendState.colorOperation;
                }
            }
        }
        else
        {
            colorBlendAttachment[0] = {};
            colorBlendAttachment[0].blendEnable = VK_FALSE;
            colorBlendAttachment[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        }

        VkPipelineColorBlendStateCreateInfo colorBlending{VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = creation.blendState.activeStates ? creation.blendState.activeStates : 1;
        colorBlending.pAttachments = colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        pipelineInfo.pColorBlendState = &colorBlending;

        VkPipelineDepthStencilStateCreateInfo depthStencil{VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
        depthStencil.depthWriteEnable = creation.depthStencil.depthWriteEnable ? VK_TRUE : VK_FALSE;
        depthStencil.stencilTestEnable = creation.depthStencil.stencilEnable ? VK_TRUE : VK_FALSE;
        depthStencil.depthTestEnable = creation.depthStencil.depthEnable ? VK_TRUE : VK_FALSE;
        depthStencil.depthCompareOp = creation.depthStencil.depthCompare;

        pipelineInfo.pDepthStencilState = &depthStencil;

        VkPipelineMultisampleStateCreateInfo multisampling = {};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f;
        multisampling.pSampleMask = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable = VK_FALSE;

        pipelineInfo.pMultisampleState = &multisampling;

        VkPipelineRasterizationStateCreateInfo rasterizer{VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = creation.rasterization.cullMode;
        rasterizer.frontFace = creation.rasterization.frontFace;
        rasterizer.depthBiasEnable = VK_FALSE;

        pipelineInfo.pRasterizationState = &rasterizer;

        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_swapchainWidth);
        viewport.height = static_cast<float>(m_swapchainHeight);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor = {};
        scissor.offset = {0, 0};
        scissor.extent = {m_swapchainWidth, m_swapchainHeight};

        VkPipelineViewportStateCreateInfo viewportState{VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.renderPass = GetVulkanRenderPass(creation.renderPass, creation.name);

        VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        VkPipelineDynamicStateCreateInfo dynamicState{VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
        dynamicState.dynamicStateCount = sizeof(dynamicStates) / (sizeof(dynamicStates)[0]);
        dynamicState.pDynamicStates = dynamicStates;

        pipelineInfo.pDynamicState = &dynamicState;

        vkCreateGraphicsPipelines(m_vulkanDevice, VK_NULL_HANDLE, 1, &pipelineInfo, m_vulkanAllocationCallbacks, &pipeline->pipeline);
        pipeline->bindPoint = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;
    }
    else
    {
        VkComputePipelineCreateInfo pipelineInfo{VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO};
        pipelineInfo.stage = shaderStateData->stageInfos[0];
        pipelineInfo.layout = pipelineLayout;

        vkCreateComputePipelines(m_vulkanDevice, VK_NULL_HANDLE, 1, &pipelineInfo, m_vulkanAllocationCallbacks, &pipeline->pipeline);
        pipeline->bindPoint = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE;
    }

    return handle;
}

SamplerHandle GpuDevice::CreateSampler(const SamplerCreation &creation)
{
    SamplerHandle handle = {m_samplers.ObtainResource()};
    if (handle.index == k_invalidIndex)
        return handle;

    Sampler *sampler = AccessSampler(handle);
    sampler->addressU = creation.addressU;
    sampler->addressV = creation.addressV;
    sampler->addressW = creation.addressW;
    sampler->minFilter = creation.minFilter;
    sampler->magFilter = creation.magFilter;
    sampler->mipmapMode = creation.mipmapMode;
    sampler->name = creation.name;

    VkSamplerCreateInfo createInfo{VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
    createInfo.addressModeU = creation.addressU;
    createInfo.addressModeV = creation.addressV;
    createInfo.addressModeW = creation.addressW;
    createInfo.minFilter = creation.minFilter;
    createInfo.magFilter = creation.magFilter;
    createInfo.mipmapMode = creation.mipmapMode;
    createInfo.anisotropyEnable = 0;
    createInfo.compareEnable = 0;
    createInfo.unnormalizedCoordinates = 0;
    createInfo.borderColor = VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_WHITE;

    vkCreateSampler(m_vulkanDevice, &createInfo, m_vulkanAllocationCallbacks, &sampler->sampler);
    SetResourceName(VK_OBJECT_TYPE_SAMPLER, reinterpret_cast<uint64_t>(sampler->sampler), creation.name);
    
    return handle;
}
