#ifndef JSON_H
#define JSON_H

#include "Poke/Utils/JsonNode.h"
#include <filesystem>

namespace Poke
{
    class Json
    {
    public:
        static JsonNode LoadFromFile(const std::filesystem::path &path);
        static bool SaveToFile(const JsonNode &node, const std::filesystem::path &path);
    };

}

#endif