#ifndef MODEL_H
#define MODEL_H

#include "Poke/Resources/Asset.h"

#include <glm/glm.hpp>

#include <string>
#include <vector>

namespace Poke
{
    class Model : public Asset
    {
    public:
        struct Node
        {
            std::string name;
            glm::mat4 localTransform{1.0f};

            std::vector<AssetHandle> meshes;
            std::vector<Node> children;
        };

    public:
        Model();
        Model(AssetHandle handle);
        Model(AssetHandle handle, Model::Node root);

        AssetType GetType() const override { return AssetType::Model; }

        const Node &GetRootNode() const { return m_rootNode; }

    private:
        Node m_rootNode;
    };
}

#endif