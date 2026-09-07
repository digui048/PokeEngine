#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Component.h"
#include <string>
#include <vector>
#include <memory>

namespace Poke
{
    class GameObject
    {
    public:
        GameObject(const std::string &name = "GameObject", GameObject *parent = nullptr)
            : m_name(name), m_parent(parent), m_active(true) {}
        ~GameObject() = default;

        void Update(float deltaTime)
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

        void AddChild(std::unique_ptr<GameObject> child)
        {
            if (child)
            {
                child->m_parent = this;
                m_children.push_back(std::move(child));
            }
        }

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

        const std::string &GetName() const { return m_name; }
        void SetActive(bool state) { m_active = state; }
        bool IsActive() const { return m_active; }

    private:
        std::string m_name;
        bool m_active = true;

        GameObject *m_parent;
        std::vector<std::unique_ptr<GameObject>> m_children;
        std::vector<std::unique_ptr<Component>> m_components;
    };
}

#endif