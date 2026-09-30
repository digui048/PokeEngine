#ifndef BINARY_SERIALIZER_H
#define BINARY_SERIALIZER_H

#include <fstream>
#include <cstdint>

namespace Poke
{
    void WriteString(std::ofstream &file, const std::string &value)
    {
        uint32_t size = static_cast<uint32_t>(value.size());
        file.write(reinterpret_cast<const char *>(&size), sizeof(uint32_t));
        file.write(value.data(), size);
    }

    std::string ReadString(std::ifstream &file)
    {
        uint32_t size = 0;
        file.read(reinterpret_cast<char *>(&size), sizeof(uint32_t));
        std::string value(size, '\0');
        file.read(value.data(), size);

        return value;
    }
}

#endif