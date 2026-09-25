#pragma once

#include "Window.h"

class WindowManager
{
public:
    WindowManager() = default;
    ~WindowManager() = default;

    Window* CreateMainWindow(const WindowProps& props);
    Window* GetMainWindow();
    void DestroyMainWindow();

    Window* CreateWindow(const WindowProps& props);
    Window* GetWindowFromID(int WindowID);
    Window* GetWindowFromHandle(void* handle);
    void DestroyWindow(Window* window);

    static WindowManager& Get() { return *s_Instance; }

    std::vector<Window*>::iterator begin() { return m_Windows.begin(); }
    std::vector<Window*>::iterator end() { return m_Windows.end(); }
    std::vector<Window*>::reverse_iterator rbegin() { return m_Windows.rbegin(); }
    std::vector<Window*>::reverse_iterator rend() { return m_Windows.rend(); }

    std::vector<Window*>::const_iterator cbegin() { return m_Windows.begin(); }
    std::vector<Window*>::const_iterator cend() { return m_Windows.end(); }
    std::vector<Window*>::const_reverse_iterator crbegin() { return m_Windows.rbegin(); }
    std::vector<Window*>::const_reverse_iterator crend() { return m_Windows.rend(); }

private:
    Window* m_MainWindow;
    std::vector<Window*> m_Windows;

    static WindowManager* s_Instance;
};
