#ifndef SCENE_H
#define SCENE_H

#include "GameObject.h"
#include "Poke/Resources/Asset.h"
#include "Poke/Resources/Assets/Model.h"
#include <memory>

namespace Poke
{
    class Scene : public Asset
    {
    public:
        Scene();
        Scene(AssetHandle handle);
        ~Scene() override = default;

        AssetType GetType() const override { return AssetType::Scene; }

        void OnInit();
        void OnUpdate(float deltaTime);
        void OnShutdown();

        GameObject *CreateGameObject(const std::string &name);
        GameObject *CreateGameObjectWithUUID(UUID uuid, const std::string &name);
        GameObject *GetRoot() const { return m_rootGameObject.get(); }

        GameObject *InstantiateModel(AssetHandle modelHandle, const std::string &name);
        void InstantiateModelNode(const Model::Node &node, GameObject *gameObject);

    private:
        std::unique_ptr<GameObject> m_rootGameObject;
    };
}

#endif