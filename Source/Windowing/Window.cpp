#include "Window.h"

#include <iostream>

#include "WindowManager.h"

bool Window::s_OGLInitialized = false;

Window::Window(const WindowProps& props)
    : m_Props(props)
{
    GLFWmonitor* monitor = nullptr;
    if (m_Props.fullscreen)
    {
        monitor = glfwGetPrimaryMonitor();
        m_Props.width = glfwGetVideoMode(monitor)->width;
        m_Props.height = glfwGetVideoMode(monitor)->height;
    }

    m_Window = glfwCreateWindow(m_Props.width, m_Props.height, m_Props.title.c_str(), monitor, nullptr);
    if (!m_Window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwSetKeyCallback(m_Window, KeyCallback);
    glfwSetCursorPosCallback(m_Window, CursorPosCallback);
    glfwSetScrollCallback(m_Window, MouseScrollCallback);
    glfwSetFramebufferSizeCallback(m_Window, FramebufferResizeCallback);

    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(1);

    if (!s_OGLInitialized)
    {
        s_OGLInitialized = true;
        int version = gladLoadGL(glfwGetProcAddress);
        if (version == 0)
        {
            std::cout << "Failed to initialize OpenGL context" << std::endl;
            return;
        }

        glDebugMessageCallback(GLErrorCallback, nullptr);
    }
}

void Window::SetTitle(const std::string& title)
{
    m_Props.title = title;
    glfwSetWindowTitle(m_Window, title.c_str());
}

void Window::SetFullscreen(bool fullscreen)
{
    if (fullscreen)
    {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(m_Window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        m_Props.fullscreen = true;
    }
    else
    {
        glfwSetWindowMonitor(m_Window, nullptr, 0, 0, 1920, 1080, GLFW_DONT_CARE);
        m_Props.fullscreen = false;
    }
}

void Window::PollEvents()
{
    glfwPollEvents();
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(m_Window);
}

void Window::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Window* p_Window = WindowManager::Get().GetWindowFromHandle(window);
    p_Window->GetKeyCallback()(p_Window, key, scancode, action, mods);
}

void Window::CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    Window* p_Window = WindowManager::Get().GetWindowFromHandle(window);
    p_Window->GetCursorPosCallback()(p_Window, xpos, ypos);
}

void Window::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    Window* p_Window = WindowManager::Get().GetWindowFromHandle(window);
    p_Window->GetMouseScrollCallback()(p_Window, xoffset, yoffset);
}

void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    Window* p_Window = WindowManager::Get().GetWindowFromHandle(window);
    p_Window->m_FramebufferResizeCallback(p_Window, width, height);
}

void Window::GLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    std::cout << std::format("GL CALLBACK: {0}, type = {1}, severity = {2}, message = {3}", type == GL_DEBUG_TYPE_ERROR ? "GL_ERROR" : "", type, severity, message) << std::endl;
}
