#ifndef MODEL_IMPORTER_H
#define MODEL_IMPORTER_H

#include "Poke/Resources/Assets/Model.h"
#include "Poke/Resources/AssetMetaData.h"
#include "Poke/Resources/AssetImportResult.h"

#include <assimp/scene.h>

#include <memory>
#include <string>
#include <vector>

namespace Poke
{
    class ModelImporter
    {
    public:
        ModelImporter() = delete;

        static AssetImportResult LoadModel(const AssetMetaData &metadata);

    private:
        static Model::Node ProcessNode(aiNode *node, const aiScene *scene, const std::vector<AssetHandle> &meshHandles);
        static glm::mat4 ConvertMatrix(const aiMatrix4x4 &matrix);
    };
}

#endif