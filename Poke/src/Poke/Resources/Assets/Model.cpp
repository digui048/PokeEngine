#include "Model.h"

using namespace Poke;

Model::Model()
    : Model(AssetHandle())
{
}

Model::Model(AssetHandle handle)
    : Asset(handle)
{
}

Model::Model(AssetHandle handle, Node root)
    : Asset(handle), m_rootNode(std::move(root))
{
}
