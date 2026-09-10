#include "TransformComponent.h"

using namespace Poke;

TransformComponent::TransformComponent(GameObject *owner)
    : Component(owner, ComponentType::TRANSFORM)
{
}

void TransformComponent::SetDirty()
{
    m_isDirty = true;
    m_isWorldDirty = true;

    if (m_owner)
    {
        for (const auto &child : m_owner->GetChildren())
        {
            if (auto *childTransform = child->GetComponent<TransformComponent>())
            {
                childTransform->SetDirty();
            }
        }
    }
}

const glm::mat4 &Poke::TransformComponent::GetLocalTransform() const
{
    if (m_isDirty)
    {
        m_localMatrix = glm::translate(glm::mat4(1.0f), m_position) *
                        glm::toMat4(m_rotation) *
                        glm::scale(glm::mat4(1.0f), m_scale);
        m_isDirty = false;
    }
    return m_localMatrix;
}

const glm::mat4 &Poke::TransformComponent::GetWorldTransform() const
{
    if (m_isWorldDirty || m_isDirty)
    {
        if (m_owner && m_owner->GetParent())
        {
            if (auto *parentTransform = m_owner->GetParent()->GetComponent<TransformComponent>())
            {
                m_worldMatrix = parentTransform->GetWorldTransform() * GetLocalTransform();
            }
            else
            {
                m_worldMatrix = GetLocalTransform();
            }
        }
        else
        {
            m_worldMatrix = GetLocalTransform();
        }
        m_isWorldDirty = false;
    }
    return m_worldMatrix;
}

void TransformComponent::SetLocalTransform(const glm::mat4 &transform)
{
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(transform, m_scale, m_rotation, m_position, skew, perspective);

    m_eulerDegrees = glm::degrees(glm::eulerAngles(m_rotation));
    SetDirty();
}

void TransformComponent::SetWorldTransform(const glm::mat4 &worldtransform)
{
    if (m_owner && m_owner->GetParent())
    {
        if (auto *parentTransform = m_owner->GetParent()->GetComponent<TransformComponent>())
        {
            glm::mat4 invParent = glm::inverse(parentTransform->GetWorldTransform());
            SetLocalTransform(invParent * worldtransform);
            return;
        }
    }
    SetLocalTransform(worldtransform);
}

void TransformComponent::SetLocalTranslation(const glm::vec3 &position)
{
    m_position = position;
    SetDirty();
}

glm::vec3 TransformComponent::GetWorldTranslation() const
{
    return glm::vec3(GetWorldTransform()[3]);
}

void TransformComponent::SetWorldTranslation(const glm::vec3 &worldPosition)
{
    if (m_owner && m_owner->GetParent())
    {
        if (auto *parentTransform = m_owner->GetParent()->GetComponent<TransformComponent>())
        {
            glm::mat4 invParent = glm::inverse(parentTransform->GetWorldTransform());
            m_position = glm::vec3(invParent * glm::vec4(worldPosition, 1.0f));
            SetDirty();
            return;
        }
    }
    SetLocalTranslation(worldPosition);
}

void TransformComponent::SetLocalRotation(const glm::quat &rotation)
{
    glm::quat deltaQuat = rotation * glm::inverse(m_rotation);
    glm::vec3 deltaEuler = glm::eulerAngles(deltaQuat);
    glm::vec3 eulerRad = glm::radians(m_eulerDegrees) + deltaEuler;

    for (int i = 0; i < 3; ++i)
    {
        if (eulerRad[i] > glm::pi<float>())
            eulerRad[i] -= glm::two_pi<float>();
        if (eulerRad[i] < -glm::pi<float>())
            eulerRad[i] += glm::two_pi<float>();
    }

    m_eulerDegrees = glm::degrees(eulerRad);
    m_rotation = rotation;
    SetDirty();
}

void TransformComponent::SetLocalRotationEuler(const glm::vec3 &degrees)
{
    m_eulerDegrees = degrees;
    m_rotation = glm::quat(glm::radians(m_eulerDegrees));
    SetDirty();
}

glm::quat TransformComponent::GetWorldRotation() const
{
    glm::vec3 scale, translation, skew;
    glm::vec4 perspective;
    glm::quat worldRotation;
    glm::decompose(GetWorldTransform(), scale, worldRotation, translation, skew, perspective);
    return worldRotation;
}

glm::vec3 TransformComponent::GetWorldRotationEuler() const
{
    return glm::degrees(glm::eulerAngles(GetWorldRotation()));
}

void TransformComponent::SetLocalScale(const glm::vec3 &scale)
{
    m_scale = scale;
    SetDirty();
}

glm::vec3 Poke::TransformComponent::GetWorldScale() const
{
    glm::vec3 scale, translation, skew;
    glm::vec4 perspective;
    glm::quat rotation;
    glm::decompose(GetWorldTransform(), scale, rotation, translation, skew, perspective);
    return scale;
}
