#pragma once

#include <cstdint>
#include <functional>
#include <string>

#include "glad/gl.h"
#include "GLFW/glfw3.h"

struct WindowProps
{
    std::string title;
    uint32_t width, height;

    bool fullscreen = false;
};

class Window
{
public:
    Window(const WindowProps& props);

    void SetTitle(const std::string& title);
    bool IsFullscreen() const { return m_Props.fullscreen; }
    void SetFullscreen(bool fullscreen);
    void SetKeyCallback(const std::function<void(Window*, int, int, int, int)>& func) { m_KeyCallback = func; }
    std::function<void(Window*, int, int, int, int)> GetKeyCallback() { return m_KeyCallback; }
    void SetCursorPosCallback(const std::function<void(Window*, double, double)>& func) { m_CursorPosCallback = func; }
    std::function<void(Window*, double, double)> GetCursorPosCallback() { return m_CursorPosCallback; }
    void SetMouseScrollCallback(const std::function<void(Window*, double, double)>& func) { m_MouseScrollCallback = func; }
    std::function<void(Window*, double, double)> GetMouseScrollCallback() { return m_MouseScrollCallback; }
    void SetFramebufferResizeCallback(const std::function<void(Window*, int, int)>& func) { m_FramebufferResizeCallback = func; }
    std::function<void(Window*, int, int)> GetFramebufferResizeCallback() { return m_FramebufferResizeCallback; }

    uint32_t GetID() { return reinterpret_cast<uint64_t>(m_Window); }
    void* GetHandle() { return m_Window; }

    void PollEvents();
    void SwapBuffers();

    void SetShouldClose() { glfwSetWindowShouldClose(m_Window, GLFW_TRUE); }
    bool ShouldClose() const { return glfwWindowShouldClose(m_Window); }

    GLFWwindow* GetNativeWindow() { return m_Window; }

public:
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
    static void APIENTRY GLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

private:
    WindowProps m_Props;
    GLFWwindow* m_Window;

    std::function<void(Window*, int, int, int, int)> m_KeyCallback;
    std::function<void(Window*, double, double)> m_CursorPosCallback;
    std::function<void(Window*, double, double)> m_MouseScrollCallback;
    std::function<void(Window*, int, int)> m_FramebufferResizeCallback;

    static bool s_OGLInitialized;
};