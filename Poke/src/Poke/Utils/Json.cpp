#include "Json.h"
#include "Poke/Core/Log.h"
#include <fstream>

using namespace Poke;

JsonNode Json::LoadFromFile(const std::filesystem::path &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        POKE_CORE_ERROR("[Json] Failed to open file: {0}", path.string());
        return JsonNode();
    }

    nlohmann::json j;
    file >> j;
    return JsonNode(j);
}

bool Json::SaveToFile(const JsonNode &node, const std::filesystem::path &path)
{
    std::filesystem::create_directories(path.parent_path());
    std::ofstream file(path);
    if (!file.is_open())
        return false;

    file << node.GetInternal().dump(4);
    return true;
}
