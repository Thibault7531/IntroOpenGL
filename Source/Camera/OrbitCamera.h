#pragma once
#include "../Windowing/Window.h"
#include "glm/glm.hpp"

class OrbitCamera
{
public:
    OrbitCamera() = default;
    OrbitCamera(float FOV, float aspectRatio, float nearClip, float farClip);
    ~OrbitCamera() = default;

    float GetDistance() const { return m_Distance; }
    void SetDistance(float distance) { m_Distance = distance; RecalculateViewMatrix(); }

    void SetViewportSize(float width, float height);

    const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
    const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
    glm::mat4 GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }

    glm::vec3 GetUpVector() const;
    glm::vec3 GetRightVector() const;
    glm::vec3 GetForwardVector() const;
    const glm::vec3& GetPosition() const { return m_Position; }
    glm::quat GetOrientation() const;

    float GetPitch() const { return m_Pitch; }
    void SetPitch(float pitch) { m_Pitch = pitch; RecalculateViewMatrix(); }
    float GetYaw() const { return m_Yaw; }
    void SetYaw(float yaw) { m_Yaw = yaw; RecalculateViewMatrix(); }

    void OnWindowResized(float width, float height);
    void OnMouseMoved(Window* window, float x, float y);
    void OnMouseScrolled(float xOffset, float yOffset);

private:
    void RecalculateViewMatrix();
    void RecalculateProjectionMatrix();

    glm::vec3 CalculatePosition();

    void MousePan(const glm::vec2& delta);
    void MouseRotate(const glm::vec2& delta);
    void MouseZoom(float delta);

    std::pair<float, float> PanSpeed() const;
    float RotationSpeed() const { return 0.008f; }
    float ZoomSpeed() const;

private:
    float m_ViewportWidth = 1920.0f, m_ViewportHeight = 1080.0f;

    float m_VerticalFOV = glm::radians(60.0f);
    float m_NearClip = 0.001f, m_FarClip = 1000.0f;
    float m_AspectRatio = 16.0f / 9.0f;

    glm::mat4 m_ProjectionMatrix = 1.0f;
    glm::mat4 m_ViewMatrix = 1.0f;
    glm::vec3 m_Position = glm::vec3(0.0f);
    glm::vec3 m_FocalPoint = glm::vec3(0.0f);

    glm::vec2 m_LastMousePosition = glm::vec2(0.0f);

    float m_Distance = 10.0f;
    float m_Pitch = 0.0f;
    float m_Yaw = 0.0f;
};
