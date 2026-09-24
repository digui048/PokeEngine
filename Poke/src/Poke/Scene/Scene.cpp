#include "Scene.h"

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
