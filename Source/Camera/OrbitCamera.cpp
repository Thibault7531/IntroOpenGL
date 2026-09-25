#include "OrbitCamera.h"

#include "glm/detail/type_quat.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "GLFW/glfw3.h"

OrbitCamera::OrbitCamera(float FOV, float aspectRatio, float nearClip, float farClip)
    : m_VerticalFOV{FOV}, m_AspectRatio{aspectRatio}, m_NearClip{nearClip}, m_FarClip{farClip}
{
    RecalculateViewMatrix();
    RecalculateProjectionMatrix();
}

void OrbitCamera::SetViewportSize(float width, float height)
{
    m_AspectRatio = width / height;
    m_ViewportWidth = width;
    m_ViewportHeight = height;
    RecalculateProjectionMatrix();
}

glm::vec3 OrbitCamera::GetUpVector() const
{
    return glm::rotate(GetOrientation(), glm::vec3{0.0f, 1.0f, 0.0f});
}

glm::vec3 OrbitCamera::GetRightVector() const
{
    return glm::rotate(GetOrientation(), glm::vec3{1.0f, 0.0f, 0.0f});
}

glm::vec3 OrbitCamera::GetForwardVector() const
{
    return glm::rotate(GetOrientation(), glm::vec3{0.0f, 0.0f, -1.0f});
}

glm::quat OrbitCamera::GetOrientation() const
{
    return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
}

void OrbitCamera::OnWindowResized(float width, float height)
{
    SetViewportSize(width, height);
}

void OrbitCamera::OnMouseMoved(Window* window, float x, float y)
{
    glm::vec2 delta = glm::vec2(x - m_LastMousePosition.x, y - m_LastMousePosition.y);

    if (glfwGetMouseButton(window->GetNativeWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        MouseRotate(delta);
        RecalculateViewMatrix();
    }
    if (glfwGetMouseButton(window->GetNativeWindow(), GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
    {
        MousePan(delta);
        RecalculateViewMatrix();
    }
    if (glfwGetMouseButton(window->GetNativeWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        MouseZoom(delta.y * 0.05f);
        RecalculateViewMatrix();
    }

    m_LastMousePosition = glm::vec2(x, y);
}

void OrbitCamera::OnMouseScrolled(float xOffset, float yOffset)
{
    MouseZoom(yOffset * 0.5f);
    RecalculateViewMatrix();
}

void OrbitCamera::MousePan(const glm::vec2& delta)
{
    auto [xSpeed, ySpeed] = PanSpeed();
    m_FocalPoint += -GetRightVector() * delta.x * xSpeed * m_Distance;
    m_FocalPoint += GetUpVector() * delta.y * ySpeed * m_Distance;
}

void OrbitCamera::MouseRotate(const glm::vec2& delta)
{
    float yawSign = GetUpVector().y < 0 ? -1.0f : 1.0f;
    m_Yaw += yawSign * delta.x * RotationSpeed();
    m_Pitch += delta.y * RotationSpeed();
}

void OrbitCamera::MouseZoom(float delta)
{
    m_Distance -= delta * ZoomSpeed();
    if (m_Distance < 1.0f) m_Distance = 1.0f;
}

std::pair<float, float> OrbitCamera::PanSpeed() const
{
    float x = std::min(m_ViewportWidth / 1000, 1.0f);
    float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;
    xFactor /= 100;

    float y = std::min(m_ViewportHeight / 1000, 1.0f);
    float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;
    yFactor /= 100;

    return { xFactor, yFactor };
}

float OrbitCamera::ZoomSpeed() const
{
    float distance = m_Distance * 0.2f;
    distance = std::max(distance, 0.0f);

    float speed = distance * distance;
    speed = std::min(speed, 100.0f);

    return speed;
}

void OrbitCamera::RecalculateViewMatrix()
{
    m_Position = CalculatePosition();
    glm::quat orientation = GetOrientation();

    m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
    m_ViewMatrix = glm::inverse(m_ViewMatrix);
}

void OrbitCamera::RecalculateProjectionMatrix()
{
    m_ProjectionMatrix = glm::perspective(m_VerticalFOV, m_AspectRatio, m_NearClip, m_FarClip);
}

glm::vec3 OrbitCamera::CalculatePosition()
{
    return m_FocalPoint - GetForwardVector() * m_Distance;
}
