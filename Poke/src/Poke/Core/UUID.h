#ifndef UUID_H
#define UUID_H

#include <cstdint>
#include <functional>

namespace Poke
{
    class UUID
    {
    public:
        UUID();
        UUID(uint64_t uuid);
        UUID(const UUID &) = default;

        operator uint64_t() const { return m_uuid; }
        bool operator==(const UUID &other) const;

    private:
        uint64_t m_uuid;
    };

    inline uint64_t GenerateSubUUID(uint64_t parentUUID, uint32_t subIndex)
    {
        return parentUUID ^ (std::hash<uint32_t>()(subIndex) + 0x9e3779b9 + (parentUUID << 6) + (parentUUID >> 2));
    }
}

namespace std
{
    template <>
    struct hash<Poke::UUID>
    {
        std::size_t operator()(const Poke::UUID &uuid) const
        {
            return static_cast<std::size_t>(uuid);
        }
    };
}

#endif