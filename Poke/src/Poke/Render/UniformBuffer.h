#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>
#include <vector>

namespace Poke
{
    struct CameraData
    {
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };

    struct ObjectData
    {
        glm::mat4 model;
    };

    class UniformBuffer
    {
    public:
        UniformBuffer(uint32_t size);
        ~UniformBuffer();

        void SetData(const void *data);

        VkBuffer GetBuffer(uint32_t frameIndex) const { return m_buffers[frameIndex]; }
        uint32_t GetSize() const { return m_size; }

    private:
        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    private:
        std::vector<VkBuffer> m_buffers;
        std::vector<VkDeviceMemory> m_memory;
        std::vector<void *> m_mapped;

    private:
        uint32_t m_size;
    };
}

#endif