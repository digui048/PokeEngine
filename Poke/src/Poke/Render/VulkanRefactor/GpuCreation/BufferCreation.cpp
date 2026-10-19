#include "BufferCreation.h"

using namespace Poke;

BufferCreation &BufferCreation::Reset()
{
    size = 0;
    initialData = nullptr;

    return *this;
}

BufferCreation &BufferCreation::Set(VkBufferUsageFlags flags, ResourceUsageType::Enum usage, uint32_t size)
{
    typeFlags = flags;
    this->usage = usage;
    this->size = size;

    return *this;
}

BufferCreation &BufferCreation::SetData(void *data)
{
    initialData = data;

    return *this;
}

BufferCreation &BufferCreation::SetName(const char *name)
{
    this->name = name;

    return *this;
}
