#pragma once

#include <glm/glm.hpp>

class PerspectiveCamera
{
public:
    PerspectiveCamera(float FOV, float aspectRatio);

    [[nodiscard]] const glm::mat4& getProjectionMatrix() const { return m_ProjectionMatrix; }
    [[nodiscard]] const glm::mat4& getViewMatrix() const { return m_ViewMatrix; }
    [[nodiscard]] const glm::mat4& getViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

    [[nodiscard]] const glm::vec3& getPosition() const { return m_Position; }
    void setPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }

    void SetProjection(float FOV, float aspectRatio);

    [[nodiscard]] float GetAspectRatio() const { return m_AspectRatio; }
    void SetAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio; RecalculateProjectionMatrix(); }

    [[nodiscard]] float GetFOV() const { return m_FOV; }
    void SetFOV(const float FOV) { m_FOV = FOV; RecalculateProjectionMatrix(); }

    [[nodiscard]] const glm::vec3& GetRotation() const { return m_Rotation; }
    void SetRotation(const glm::vec3& rotation);

private:
    void RecalculateViewMatrix();
    void RecalculateProjectionMatrix();

private:
    glm::mat4 m_ProjectionMatrix = 1.0f;
    glm::mat4 m_ViewMatrix = 1.0f;
    glm::mat4 m_ViewProjectionMatrix = 1.0f;

    float m_AspectRatio = 16.0f/9.0f;
    float m_FOV = glm::radians(60.0f);

    glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
    glm::vec3 m_Rotation = { 0.0f, -90.0f, 0.0f };
};

