#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Poke/Core/UUID.h"
#include "Component.h"
#include <string>
#include <vector>
#include <memory>

namespace Poke
{
    class TransformComponent;

    class GameObject
    {
    public:
        GameObject(const std::string &name = "GameObject", GameObject *parent = nullptr);
        GameObject(UUID uuid, const std::string &name = "GameObject", GameObject *parent = nullptr);
        ~GameObject() = default;

        TransformComponent *GetTransform() const { return m_transform; }

        void Update(float deltaTime);
        void AddChild(std::unique_ptr<GameObject> child);

        GameObject *GetParent() const { return m_parent; }
        const std::vector<std::unique_ptr<GameObject>> &GetChildren() const { return m_children; }

        template <typename T, typename... Args>
        T *AddComponent(Args &&...args)
        {
            auto component = std::make_unique<T>(this, std::forward<Args>(args)...);
            T *ref = component.get();
            m_components.push_back(std::move(component));
            return ref;
        }

        template <typename T>
        T *GetComponent()
        {
            for (auto &component : m_components)
            {
                if (T *typedComponent = dynamic_cast<T *>(component.get()))
                    return typedComponent;
            }
            return nullptr;
        }

        const std::vector<std::unique_ptr<Component>> &GetComponents() { return m_components; }

        UUID GetUUID() const { return m_uuid; }
        const std::string &GetName() const { return m_name; }
        void SetActive(bool state) { m_active = state; }
        bool IsActive() const { return m_active; }

    private:
        UUID m_uuid;
        std::string m_name;
        bool m_active = true;

        GameObject *m_parent;
        TransformComponent *m_transform = nullptr;
        std::vector<std::unique_ptr<GameObject>> m_children;
        std::vector<std::unique_ptr<Component>> m_components;
    };
}

#endif