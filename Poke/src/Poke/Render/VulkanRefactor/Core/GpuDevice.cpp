#include "GpuDevice.h"
#include "Poke/Core/Log.h"
#include "Poke/Core/Assert.h"

#include "../Command/CommandBufferRing.h"

#include <SDL3/SDL_vulkan.h>

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
    return false
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

void GpuDevice::Init(const DeviceCreation &creation)
{
    POKE_CORE_INFO("[GpuDevice] Gpu Device initializing");

    VkResult result;

    VkApplicationInfo applicationInfo = {VK_STRUCTURE_TYPE_APPLICATION_INFO, nullptr, "Poke Graphics Device", 1, "PokeEngine", 1, VK_MAKE_VERSION(1, 2, 0)};
    VkInstanceCreateInfo createInfo = IsValidationsEnabled() ? createInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, nullptr, 0, &applicationInfo, sizeof(s_requestedLayers) / sizeof((s_requestedLayers)[0]), s_requestedLayers, sizeof(s_requestedExtensions) / sizeof((s_requestedExtensions)[0]), s_requestedExtensions}
                                                             : createInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, nullptr, 0, &applicationInfo, 0, nullptr, 0, nullptr};
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

    uint32_t numPhysicalDevice;
    result = vkEnumeratePhysicalDevices(m_vulkanInstance, &numPhysicalDevice, NULL);
    POKE_ASSERT(result == VK_SUCCESS, "Failed to enumerate physical devices");
    std::vector<VkPhysicalDevice> gpus(numPhysicalDevice);
    result = vkEnumeratePhysicalDevices(m_vulkanInstance, &numPhysicalDevice, gpus.data());
    POKE_ASSERT(result == VK_SUCCESS, "Failed to enumerate physical devices");

    SDL_Window *window = (SDL_Window *)creation.window;
    if (!SDL_Vulkan_CreateSurface(window, m_vulkanInstance, nullptr, &m_vulkanWindowSurface))
        POKE_CORE_ERROR("[GpuDevice] Failed to create Vulkan surface");

    s_sdlWindow = window;

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

    result = vkCreateDevice(m_vulkanPhysicalDevice, &deviceCreateInfo, m_vulkanAllocationCallbacks, &m_vulkanDevice);
    POKE_ASSERT(result == VK_SUCCESS, "Failed to create logical device");

    if (m_debugUtilsExtensionPresent)
    {
        pfnSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetDeviceProcAddr(m_vulkanDevice, "vkSetDebugUtilsObjectNameEXT");
        pfnCmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetDeviceProcAddr(m_vulkanDevice, "vkCmdBeginDebugUtilsLabelEXT");
        pfnCmdEndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetDeviceProcAddr(m_vulkanDevice, "vkCmdEndDebugUtilsLabelEXT");
    }

    vkGetDeviceQueue(m_vulkanDevice, m_vulkanQueueFamily, 0, &m_vulkanQueue);

    int windowW, windowH;
    SDL_GetWindowSize(window, &windowW, &windowH);

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

    CreateSwapchain();

    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = m_vulkanPhysicalDevice;
    allocatorInfo.device = m_vulkanDevice;
    allocatorInfo.instance = m_vulkanInstance;

    result = vmaCreateAllocator(&allocatorInfo, &m_vmaAllocator);
    POKE_ASSERT(result == VK_SUCCESS, "Failed to create VMA allocator");

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
    result = vkCreateDescriptorPool(m_vulkanDevice, &poolInfo, m_vulkanAllocationCallbacks, &m_vulkanDescriptorPool);
    POKE_ASSERT(result == VK_SUCCESS, "Failed to create descriptor pool");

    m_buffers.Init(4096, sizeof(Buffer));
    m_textures.Init(512, sizeof(Texture));
    m_renderPasses.Init(256, sizeof(RenderPass));
    m_descriptorSetLayouts.Init(128, sizeof(DescriptorSetLayout));
    m_pipelines.Init(128, sizeof(Pipeline));
    m_shaders.Init(128, sizeof(ShaderState));
    m_descriptorSets.Init(128, sizeof(DescriptorSet));
    m_samplers.Init(32, sizeof(Sampler));

    VkSemaphoreCreateInfo semaphoreInfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    for (size_t i = 0; i < kMaxSwapchainImages; ++i)
    {
        vkCreateSemaphore(m_vulkanDevice, &semaphoreInfo, m_vulkanAllocationCallbacks, &m_vulkanRenderCompleteSemaphore[i]);
        vkCreateSemaphore(m_vulkanDevice, &semaphoreInfo, m_vulkanAllocationCallbacks, &m_vulkanImageAcquiredSemaphore[i]);

        VkFenceCreateInfo fenceInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        vkCreateFence(m_vulkanDevice, &fenceInfo, m_vulkanAllocationCallbacks, &m_vulkanCommandBufferExecutedFence[i]);
    }

    s_commandBufferRing.Init(this);
    m_queuedCommandBuffers.reserve(128);

    m_vulkanImageIndex = 0;
    m_currentFrame = 1;
    m_previousFrame = 0;
    m_absoluteFrame = 0;

    m_resourceDeletionQueue.reserve(16);
    m_descriptorSetUpdates.reserve(16);
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

    RenderPass* vkSwapchainPass = AccessRenderPass(m_swapchainPass);
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
    return BufferHandle();
}
