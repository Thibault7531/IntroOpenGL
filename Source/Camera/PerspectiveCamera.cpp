#include "PerspectiveCamera.h"

#include <glm/gtc/matrix_transform.hpp>

PerspectiveCamera::PerspectiveCamera(const float FOV, const float aspectRatio)
    : m_AspectRatio(aspectRatio), m_FOV(FOV)
{
    RecalculateProjectionMatrix();
    RecalculateViewMatrix();
}

void PerspectiveCamera::SetProjection(const float FOV, const float aspectRatio, float near, float far)
{
    m_Near = near;
    m_Far = far;
    m_FOV = FOV;
    m_AspectRatio = aspectRatio;
    RecalculateProjectionMatrix();
}

void PerspectiveCamera::SetRotation(const glm::vec3& rotation)
{
    m_Rotation = rotation;

    m_Rotation.x = std::max(m_Rotation.x, -89.9f);
    m_Rotation.x = std::min(m_Rotation.x, 89.9f);

    RecalculateViewMatrix();
}

void PerspectiveCamera::RecalculateProjectionMatrix()
{
    m_ProjectionMatrix = glm::perspective(m_FOV, m_AspectRatio, m_Near, m_Far);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void PerspectiveCamera::RecalculateViewMatrix()
{
    glm::vec3 direction;
    direction.x = cos(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x));
    direction.y = sin(glm::radians(m_Rotation.x));
    direction.z = sin(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x));
    direction = glm::normalize(direction);

    m_ViewMatrix = glm::lookAt(m_Position, m_Position + direction, glm::vec3(0, 1, 0));
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}
