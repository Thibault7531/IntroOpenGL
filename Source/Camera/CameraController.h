#pragma once

#include "PerspectiveCamera.h"
#include "../Windowing/Window.h"

class PerspectiveCameraController
{
public:
    PerspectiveCameraController(float aspectRatio, float FOV);

    PerspectiveCamera& GetCamera() { return m_Camera; }
    const PerspectiveCamera& GetCamera() const { return m_Camera; }

    void OnUpdate(Window* window, double deltaTime);

    void OnMouseMoveEvent(Window* window, double xPos, double yPos);
    bool OnWindowResize(uint32_t width, uint32_t height);

private:
    float m_aspectRatio = 16.0f/9.0f;
    float m_FOV = 60.0f;
    PerspectiveCamera m_Camera;

    float m_CameraTranslationSpeed = 1.0f;

    glm::vec2 m_PreviousMousePosition = { 0.0f, 0.0f };
};
