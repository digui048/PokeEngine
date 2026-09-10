#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include "Poke/Scene/Component.h"
#include "Poke/Scene/GameObject.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace Poke
{
    class TransformComponent : public Component
    {
    public:
        TransformComponent(GameObject *owner);
        ~TransformComponent() override = default;

        void Update(float dt) override {}

        void SetDirty();
        const glm::mat4 &GetLocalTransform() const;
        const glm::mat4 &GetWorldTransform() const;
        void SetLocalTransform(const glm::mat4 &transform);
        void SetWorldTransform(const glm::mat4 &worldtransform);

        const glm::vec3 &GetLocalTranslation() const { return m_position; }
        void SetLocalTranslation(const glm::vec3 &position);
        glm::vec3 GetWorldTranslation() const;
        void SetWorldTranslation(const glm::vec3 &worldPosition);
        void MovePosition(const glm::vec3& deltaWorldOffset);

        const glm::quat &GetLocalRotation() const { return m_rotation; }
        void SetLocalRotation(const glm::quat &rotation);
        const glm::vec3 &GetLocalRotationEuler() const { return m_eulerDegrees; }
        void SetLocalRotationEuler(const glm::vec3 &degrees);
        glm::quat GetWorldRotation() const;
        glm::vec3 GetWorldRotationEuler() const;
        void RotateLocalX(float angleRadians);
        void RotateLocalY(float angleRadians);
        void RotateLocalZ(float angleRadians);

        const glm::vec3 &GetLocalScale() const { return m_scale; }
        void SetLocalScale(const glm::vec3 &scale);
        glm::vec3 GetWorldScale() const;

    private:
        glm::vec3 m_position{0.0f, 0.0f, 0.0f};
        glm::quat m_rotation{1.0f, 0.0f, 0.0f, 0.0f};
        glm::vec3 m_eulerDegrees{0.0f, 0.0f, 0.0f};
        glm::vec3 m_scale{1.0f, 1.0f, 1.0f};

        mutable bool m_isDirty = true;
        mutable bool m_isWorldDirty = true;
        mutable glm::mat4 m_localMatrix{1.0f};
        mutable glm::mat4 m_worldMatrix{1.0f};
    };
}

#endif