#include "GameObject.h"
#include "Poke/Scene/Components/TransformComponent.h"

using namespace Poke;

GameObject::GameObject(const std::string &name, GameObject *parent)
    : m_uuid(), m_name(name), m_parent(parent), m_active(true)
{
    m_transform = AddComponent<TransformComponent>();
}

GameObject::GameObject(UUID uuid, const std::string &name, GameObject *parent)
    : m_uuid(uuid), m_name(name), m_parent(parent), m_active(true)
{
    m_transform = AddComponent<TransformComponent>();
}

void GameObject::Update(float deltaTime)
{
    if (!m_active)
        return;

    for (auto &component : m_components)
    {
        if (component->IsActive())
        {
            component->Update(deltaTime);
        }
    }

    for (auto &child : m_children)
    {
        if (child->IsActive())
        {
            child->Update(deltaTime);
        }
    }
}

void GameObject::AddChild(std::unique_ptr<GameObject> child)
{
    if (child)
    {
        child->m_parent = this;
        child->GetTransform()->SetDirty();
        m_children.push_back(std::move(child));
    }
}
