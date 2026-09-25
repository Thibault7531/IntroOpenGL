#include "CameraController.h"

#include "../Windowing/Window.h"

PerspectiveCameraController::PerspectiveCameraController(float aspectRatio, float FOV)
    : m_aspectRatio(aspectRatio), m_FOV(FOV), m_Camera(PerspectiveCamera(m_FOV, m_aspectRatio))
{
}

void PerspectiveCameraController::OnUpdate(Window* window, double deltaTime)
{
    glm::vec3 cameraPos = m_Camera.getPosition();
    glm::vec3 cameraRotation = m_Camera.GetRotation();

    if (glfwGetKey(window->GetNativeWindow(), GLFW_KEY_W) == GLFW_PRESS) {
        cameraPos += glm::vec3(2.0f * glm::cos(glm::radians(cameraRotation.y)) * deltaTime, 0.0f, 2.0f * glm::sin(glm::radians(cameraRotation.y)) * deltaTime);
    }
    if (glfwGetKey(window->GetNativeWindow(), GLFW_KEY_S) == GLFW_PRESS) {
        cameraPos += glm::vec3(-2.0f * glm::cos(glm::radians(cameraRotation.y)) * deltaTime, 0.0f, -2.0f * glm::sin(glm::radians(cameraRotation.y)) * deltaTime);
    }
    if (glfwGetKey(window->GetNativeWindow(), GLFW_KEY_D) == GLFW_PRESS) {
        cameraPos += glm::vec3(-2.0f * glm::sin(glm::radians(cameraRotation.y)) * deltaTime, 0.0f, 2.0f * glm::cos(glm::radians(cameraRotation.y)) * deltaTime);
    }
    if (glfwGetKey(window->GetNativeWindow(), GLFW_KEY_A) == GLFW_PRESS) {
        cameraPos += glm::vec3(2.0f * glm::sin(glm::radians(cameraRotation.y)) * deltaTime, 0.0f, -2.0f * glm::cos(glm::radians(cameraRotation.y)) * deltaTime);
    }
    if (glfwGetKey(window->GetNativeWindow(), GLFW_KEY_SPACE) == GLFW_PRESS) {
        cameraPos += glm::vec3(0.0f, 2.0f * deltaTime, 0.0f);
    }
    if (glfwGetKey(window->GetNativeWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        cameraPos += glm::vec3(0.0f, -2.0f * deltaTime, 0.0f);
    }

    m_Camera.setPosition(cameraPos);
}

void PerspectiveCameraController::OnMouseMoveEvent(Window* window, double xPos, double yPos)
{
    if (glfwGetMouseButton(window->GetNativeWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        glm::vec3 cameraRotation = m_Camera.GetRotation();
        cameraRotation += glm::vec3(m_PreviousMousePosition.y - yPos, xPos - m_PreviousMousePosition.x, 0.0f) * 0.1f;
        m_Camera.SetRotation(cameraRotation);
    }

    m_PreviousMousePosition = {xPos, yPos};
}

bool PerspectiveCameraController::OnWindowResize(uint32_t width, uint32_t height)
{
    m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    m_Camera.SetAspectRatio(m_aspectRatio);

    return false;
}
