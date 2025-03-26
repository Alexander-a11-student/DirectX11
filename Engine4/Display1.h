#ifndef DISPLAYWIN32_H
#define DISPLAYWIN32_H

#include <windows.h>

class Game; 

class Display1
{
public:
    Display1();
    ~Display1();

    bool Initialize(Game* game, int screenWidth, int screenHeight, bool fullscreen);
    void Shutdown();

    HWND GetHwnd() const { return m_hWnd; }
    HINSTANCE GetHInstance() const { return m_hInstance; }

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);

private:
    void RegisterWindowClass();
    void CreateApplicationWindow(int screenWidth, int screenHeight, bool fullscreen);

    HINSTANCE m_hInstance;
    HWND m_hWnd;
    WNDCLASSEX m_wc;
    LPCWSTR m_windowTitle;

    static Game* s_GameInstance;
};

#endif

