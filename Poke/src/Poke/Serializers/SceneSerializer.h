#ifndef SCENE_SERIALIZER_H
#define SCENE_SERIALIZER_H

#include "Poke/Serializers/AssetSerializer.h"
#include "Poke/Scene/Scene.h"
#include <nlohmann/json.hpp>

namespace Poke
{
    class SceneSerializer : public AssetSerializer
    {
    public:
        SceneSerializer() = default;
        ~SceneSerializer() override = default;

        bool SerializeToLibrary(const AssetMetaData &metadata, const std::shared_ptr<Asset> &asset, const std::filesystem::path &binaryPath) const override;
        std::shared_ptr<Asset> DeserializeFromLibrary(const AssetMetaData &metadata, const std::filesystem::path &binaryPath) const override;

    private:
        void SerializeGameObject(nlohmann::json &json, GameObject *gameObject) const;
        void DeserializeGameObject(const nlohmann::json &json, Scene *scene, GameObject *parent) const;
    };
}

#endif