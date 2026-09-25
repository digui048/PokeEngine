#ifndef UUID_H
#define UUID_H

#include <cstdint>

namespace std
{
    template <typename T>
    struct hash;
}

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