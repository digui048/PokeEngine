#ifndef QUEUE_FAMILY_INDICES_H
#define QUEUE_FAMILY_INDICES_H

#include <cstdint>
#include <optional>

namespace Poke
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> GraphicsFamily;

        bool IsComplete() const
        {
            return GraphicsFamily.has_value();
        }
    };
}

#endif