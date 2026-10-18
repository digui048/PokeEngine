#include "GpuResourcePool.h"

using namespace Poke;

namespace Poke
{
    static const uint32_t InvalidResource = 0xffffffff;
}

void GpuResourcePool::Init(uint32_t poolSize, uint32_t resourceSize)
{
    m_poolSize = poolSize;
    m_resourceSize = resourceSize;

    const uint32_t totalSize = poolSize * resourceSize + poolSize * sizeof(uint32_t);
    m_memory = new uint8_t[totalSize];
    m_freeIndices = (uint32_t *)(m_memory + poolSize * resourceSize);

    for (uint32_t i = 0; i < poolSize; ++i)
    {
        m_freeIndices[i] = i;
    }

    m_usedIndices = 0;
}

void GpuResourcePool::Shutdown()
{
    delete[] m_memory;
    m_memory = nullptr;
    m_freeIndices = nullptr;
    m_freeIndicesHead = 0;
    m_poolSize = 16;
    m_resourceSize = 4;
    m_usedIndices = 0;
}

uint32_t GpuResourcePool::ObtainResource()
{
    if (m_freeIndicesHead < m_poolSize)
    {
        const uint32_t index = m_freeIndices[m_freeIndicesHead++];
        ++m_usedIndices;
        return index;
    }

    return -1;
}

void GpuResourcePool::ReleaseResource(uint32_t index)
{
    m_freeIndices[--m_freeIndicesHead] = index;
    --m_usedIndices;
}

void GpuResourcePool::FreeAllResources()
{
    m_freeIndicesHead = 0;
    m_usedIndices = 0;

    for (uint32_t i = 0; i < m_poolSize; ++i)
    {
        m_freeIndices[i] = i;
    }
}

void *GpuResourcePool::AccessResource(uint32_t index)
{
    if (index != InvalidResource)
        return &m_memory[index * m_resourceSize];
    return nullptr;
}

const void *GpuResourcePool::AccessResource(uint32_t index) const
{
    if (index != InvalidResource)
        return &m_memory[index * m_resourceSize];
    return nullptr;
}