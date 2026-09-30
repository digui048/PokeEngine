#include "Scene.h"

#include "Poke/Resources/AssetManager.h"
#include "Poke/Scene/Components/TransformComponent.h"
#include "Poke/Scene/Components/MeshRendererComponent.h"

using namespace Poke;

void Scene::OnInit()
{
    m_rootGameObject = std::make_unique<GameObject>("Root");
}

void Scene::OnUpdate(float deltaTime)
{
    if (m_rootGameObject)
    {
        m_rootGameObject->Update(deltaTime);
    }
}

void Scene::OnShutdown()
{
    if (m_rootGameObject)
    {
        m_rootGameObject.reset();
    }
}

GameObject *Scene::CreateGameObject(const std::string &name)
{
    return CreateGameObjectWithUUID(UUID(), name);
}

GameObject *Scene::CreateGameObjectWithUUID(UUID uuid, const std::string &name)
{
    if (!m_rootGameObject)
    {
        OnInit();
    }

    auto newGameObject = std::make_unique<GameObject>(uuid, name, m_rootGameObject.get());
    GameObject *obj = newGameObject.get();

    m_rootGameObject->AddChild(std::move(newGameObject));

    return obj;
}

GameObject *Scene::InstantiateModel(AssetHandle modelHandle, const std::string &name)
{
    auto model = AssetManager::GetAsset<Model>(modelHandle);

    if (!model)
        return nullptr;

    GameObject *root = CreateGameObject(name);

    InstantiateModelNode(model->GetRootNode(), root);

    return root;
}

void Scene::InstantiateModelNode(const Model::Node &node, GameObject *gameObject)
{
    gameObject->GetTransform()->SetLocalTransform(node.localTransform);

    for (AssetHandle meshHandle : node.meshes)
    {
        auto mesh = AssetManager::GetAsset<Mesh>(meshHandle);

        if (!mesh)
            continue;

        gameObject->AddComponent<MeshRendererComponent>(mesh);
    }

    for (const Model::Node &childNode : node.children)
    {
        auto child = std::make_unique<GameObject>(childNode.name);

        GameObject *childPtr = child.get();
        gameObject->AddChild(std::move(child));

        InstantiateModelNode(childNode, childPtr);
    }
}
