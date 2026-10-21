#ifndef GPU_RESOURCE_POOL_H
#define GPU_RESOURCE_POOL_H

#include <cstdint>

namespace Poke
{
    struct ResourcePool
    {
        void Init(uint32_t poolSize, uint32_t resourceSize);
        void Shutdown();

        uint32_t ObtainResource();
        void ReleaseResource(uint32_t index);
        void FreeAllResources();

        void* AccessResource(uint32_t index);
        const void* AccessResource(uint32_t index) const;

        uint8_t* m_memory = nullptr;
        uint32_t* m_freeIndices = nullptr;

        uint32_t m_freeIndicesHead = 0;
        uint32_t m_poolSize = 16;
        uint32_t m_resourceSize = 4;
        uint32_t m_usedIndices = 0;
    };
}

#endif