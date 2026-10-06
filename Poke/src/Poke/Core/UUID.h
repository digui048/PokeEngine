#ifndef UUID_H
#define UUID_H

#include <cstdint>
#include <functional>
#include <string>

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

    inline uint64_t GenerateDeterministicUUID(const std::string &identifier, uint32_t index)
    {
        std::string value = identifier + ":" + std::to_string(index);
        uint64_t hash = 14695981039346656037ull;

        for (unsigned char character : value)
        {
            hash ^= character;
            hash *= 1099511628211ull;
        }

        return hash;
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