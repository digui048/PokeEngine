#ifndef MODEL_SERIALIZER_H
#define MODEL_SERIALIZER_H

#include "Poke/Serializers/AssetSerializer.h"
#include "Poke/Resources/Assets/Model.h"

namespace Poke
{
    class ModelSerializer : public AssetSerializer
    {
    public:
        ModelSerializer() = default;
        ~ModelSerializer() override = default;

        bool SerializeToLibrary(const AssetMetaData &metadata, const std::shared_ptr<Asset> &asset, const std::filesystem::path &binaryPath) const override;
        std::shared_ptr<Asset> DeserializeFromLibrary(const AssetMetaData &metadata, const std::filesystem::path &binaryPath) const override;

    private:
        void WriteNode(std::ofstream &file, const Model::Node &node) const;
        Model::Node ReadNode(std::ifstream &file) const;
    };
}

#endif