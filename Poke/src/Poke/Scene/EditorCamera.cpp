#include "EditorCamera.h"
#include "Poke/Core/Input.h"
#include <algorithm>

using namespace Poke;

void EditorCamera::Init(int width, int height, glm::vec3 focalPoint, float distance)
{
    m_width = width;
    m_height = height;
    m_focalPoint = focalPoint;
    m_distance = distance;

    m_position = focalPoint - GetForwardVector() * m_distance;
    UpdateView();
    UpdateProjection();
}

void EditorCamera::OnUpdate(float deltaTime)
{
    glm::vec2 mouseDelta = Input::GetMouseDelta();
    bool altPressed = Input::IsKeyHeld(SDL_SCANCODE_LALT) || Input::IsKeyHeld(SDL_SCANCODE_RALT);

    if (altPressed && Input::IsMouseButtonHeld(SDL_BUTTON_LEFT))
    {
        Input::SetCursorMode(true);
        MouseRotate(mouseDelta);
    }
    else if (Input::IsMouseButtonHeld(SDL_BUTTON_MIDDLE) || (altPressed && Input::IsMouseButtonHeld(SDL_BUTTON_RIGHT)))
    {
        Input::SetCursorMode(true);
        MousePan(mouseDelta);
    }
    else if (Input::IsMouseButtonHeld(SDL_BUTTON_RIGHT))
    {
        Input::SetCursorMode(true);
        MouseRotate(mouseDelta);

        glm::vec3 moveDir{0.0f};
        if (Input::IsKeyHeld(SDL_SCANCODE_W))
            moveDir.z += 1.0f;
        if (Input::IsKeyHeld(SDL_SCANCODE_S))
            moveDir.z -= 1.0f;
        if (Input::IsKeyHeld(SDL_SCANCODE_A))
            moveDir.x -= 1.0f;
        if (Input::IsKeyHeld(SDL_SCANCODE_D))
            moveDir.x += 1.0f;
        if (Input::IsKeyHeld(SDL_SCANCODE_E))
            moveDir.y += 1.0f;
        if (Input::IsKeyHeld(SDL_SCANCODE_Q))
            moveDir.y -= 1.0f;

        float speed = 8.0f * deltaTime;
        if (Input::IsKeyHeld(SDL_SCANCODE_LSHIFT))
            speed *= 3;

        glm::vec3 forward = GetForwardVector();
        glm::vec3 right = GetRightVector();
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        m_position += (forward * moveDir.z + right * moveDir.x + up * moveDir.y) * speed;

        m_focalPoint = m_position + forward * m_distance;
    }
    else
    {
        Input::SetCursorMode(false);
    }

    if (Input::IsKeyDown(SDL_SCANCODE_F))
    {
        Focus(glm::vec3(0.0f));
    }

    UpdateView();
}

void EditorCamera::OnMouseScroll(float offsetY)
{
    MouseZoom(offsetY);
    UpdateView();
}

void EditorCamera::Resize(int width, int height)
{
    m_width = width;
    m_height = height;
    UpdateProjection();
}

void EditorCamera::Focus(const glm::vec3 &targetPosition, float distance)
{
    m_focalPoint = targetPosition;
    m_distance = distance;

    m_yaw = -90.0f;
    m_pitch = 20.0f;

    m_position = m_focalPoint - GetForwardVector() * m_distance;
    UpdateView();
}

void EditorCamera::UpdateView()
{
    m_view = glm::lookAt(m_position, m_position + GetForwardVector(), GetUpVector());
}

void EditorCamera::UpdateProjection()
{
    m_proj = glm::perspective(glm::radians(m_fov), static_cast<float>(m_width) / static_cast<float>(m_height), m_nearPlane, m_farPlane);
    m_proj[1][1] *= -1.0f;
}

void EditorCamera::MousePan(const glm::vec2 &delta)
{
    auto [speedX, speedY] = PanSpeed();
    glm::vec3 translation = -GetRightVector() * delta.x * speedX + GetUpVector() * delta.y * speedY;
    m_position += translation;
    m_focalPoint += translation;
}

void EditorCamera::MouseRotate(const glm::vec2 &delta)
{
    float sensitivity = 0.2f;
    m_yaw += delta.x * sensitivity;
    m_pitch -= delta.y * sensitivity;
    m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);
    m_position = m_focalPoint - GetForwardVector() * m_distance;
}

void EditorCamera::MouseZoom(float delta)
{
    m_distance -= delta * ZoomSpeed();

    if (m_distance < 0.1f)
    {
        m_distance = 0.1f;
    }

    m_position = m_focalPoint - GetForwardVector() * m_distance;
}

glm::vec3 EditorCamera::GetForwardVector() const
{
    glm::vec3 forward;
    forward.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    forward.y = sin(glm::radians(m_pitch));
    forward.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    return glm::normalize(forward);
}

glm::vec3 EditorCamera::GetRightVector() const
{
    return glm::normalize(glm::cross(GetForwardVector(), glm::vec3(0.0f, 1.0f, 0.0f)));
}

glm::vec3 EditorCamera::GetUpVector() const
{
    return glm::normalize(glm::cross(GetRightVector(), GetForwardVector()));
}

std::pair<float, float> EditorCamera::PanSpeed() const
{
    float x = std::min(static_cast<float>(m_width) / 1000.0f, 2.4f);
    float factorX = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

    float y = std::min(static_cast<float>(m_height) / 1000.0f, 2.4f);
    float factorY = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

    return {factorX * m_distance * 0.03f, factorY * m_distance * 0.03f};
}

float EditorCamera::ZoomSpeed() const
{
    return std::clamp(m_distance * 0.15f, 0.5f, 10.0f);
}
