#ifndef EDITOR_CAMERA_H
#define EDITOR_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Poke
{
    class EditorCamera
    {
    public:
        EditorCamera(const EditorCamera &) = delete;
        EditorCamera &operator=(const EditorCamera &) = delete;

        static EditorCamera &Get()
        {
            static EditorCamera instance;
            return instance;
        }

        void Init(int width, int height, glm::vec3 focalPoint = glm::vec3(0.0f), float distance = 5.0f);
        void OnUpdate(float deltaTime);
        void OnMouseScroll(float offsetY);
        void Resize(int width, int height);

        void Focus(const glm::vec3 &targetPosition, float distance = 5.0f);

        const glm::mat4 &GetViewMatrix() const { return m_view; }
        const glm::mat4 &GetProjectionMatrix() const { return m_proj; }
        const glm::vec3 &GetPosition() const { return m_position; }
        const glm::vec3 &GetFocalPoint() const { return m_focalPoint; }

    private:
        EditorCamera() = default;
        ~EditorCamera() = default;

        void UpdateView();
        void UpdateProjection();

        void MousePan(const glm::vec2 &delta);
        void MouseRotate(const glm::vec2 &delta);
        void MouseZoom(float delta);

        glm::vec3 GetForwardVector() const;
        glm::vec3 GetRightVector() const;
        glm::vec3 GetUpVector() const;

        std::pair<float,float> PanSpeed() const;
        float ZoomSpeed() const;

    private:
        glm::mat4 m_view{1.0f};
        glm::mat4 m_proj{1.0f};

        glm::vec3 m_position{0.0f, 0.0f, 3.0f};
        glm::vec3 m_focalPoint{0.0f, 0.0f, 0.0f};

        float m_distance = 5.0f;
        float m_yaw = -90.0f;
        float m_pitch = 20.0f;

        float m_fov = 45.0f;
        float m_nearPlane = 0.1f;
        float m_farPlane = 1000.0f;

        int m_width = 1200;
        int m_height = 720;
    };
}

#endif