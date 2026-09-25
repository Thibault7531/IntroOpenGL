#include "WindowManager.h"

WindowManager* WindowManager::s_Instance = new WindowManager;

Window* WindowManager::CreateMainWindow(const WindowProps& props)
{
    m_MainWindow = new Window(props);
    m_Windows.push_back(m_MainWindow);
    return m_MainWindow;
}

Window* WindowManager::GetMainWindow()
{
    return m_MainWindow;
}

void WindowManager::DestroyMainWindow()
{
    bool found = false;
    int32_t i = 0;
    while (!found && i < m_Windows.size())
    {
        if (m_Windows[i] == m_MainWindow)
        {
            m_Windows.erase(m_Windows.begin() + i);
            found = true;
        }
        i++;
    }
    delete m_MainWindow;
}

Window* WindowManager::CreateWindow(const WindowProps& props)
{
    Window* window = new Window(props);
    m_Windows.push_back(window);
    return window;
}

Window* WindowManager::GetWindowFromID(int WindowID)
{
    int32_t i = 0;
    while (i < m_Windows.size())
    {
        if (m_Windows[i]->GetID() == WindowID) return m_Windows[i];
        i++;
    }
    return nullptr;
}

Window* WindowManager::GetWindowFromHandle(void* handle)
{
    int32_t i = 0;
    while (i < m_Windows.size())
    {
        if (m_Windows[i]->GetHandle() == handle) return m_Windows[i];
        i++;
    }
    return nullptr;
}

void WindowManager::DestroyWindow(Window* window)
{
    int32_t i = 0;
    while (i < m_Windows.size())
    {
        if (m_Windows[i] == window)
        {
            delete window;
            m_Windows.erase(m_Windows.begin() + i);
            return;
        }
        i++;
    }
}