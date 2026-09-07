#ifndef COMPONENT_H
#define COMPONENT_H

namespace Poke
{
    class GameObject;

    enum class ComponentType
    {
        NONE = 0,
        TRANSFORM,
        MESH,
    };

    class Component
    {
    public:
        Component(GameObject *owner, ComponentType type)
            : m_owner(owner), m_type(type), m_active(true) {}
        virtual ~Component() = default;

        virtual void Enable() {}
        virtual void Update(float deltaTime) {}
        virtual void Disable() {}

        GameObject *GetOwner() const { return m_owner; }
        ComponentType GetType() const { return m_type; }
        bool IsActive() const { return m_active; }
        bool SetActive(bool state) { m_active = state; }

    protected:
        GameObject *m_owner;
        ComponentType m_type;
        bool m_active;
    };
}

#endif