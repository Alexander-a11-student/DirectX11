#include "DisplayWin32.h"
#include "Game.h"

Game* DisplayWin32::s_GameInstance = nullptr; // Инициализация статической переменной

DisplayWin32::DisplayWin32()
{
    m_hInstance = GetModuleHandle(NULL);
    m_windowTitle = L"GameEngine";
    m_hWnd = NULL;
}

DisplayWin32::~DisplayWin32()
{
}

bool DisplayWin32::Initialize(Game* game, int screenWidth, int screenHeight, bool fullscreen)
{
    s_GameInstance = game; // Сохраняем указатель на игру
    RegisterWindowClass();
    CreateApplicationWindow(screenWidth, screenHeight, fullscreen);
    return true;
}

void DisplayWin32::RegisterWindowClass()
{
    m_wc.cbSize = sizeof(WNDCLASSEX);
    m_wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    m_wc.lpfnWndProc = WndProc; // Теперь обработчик сообщений внутри DisplayWin32
    m_wc.cbClsExtra = 0;
    m_wc.cbWndExtra = 0;
    m_wc.hInstance = m_hInstance;
    m_wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    m_wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    m_wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    m_wc.lpszMenuName = NULL;
    m_wc.lpszClassName = m_windowTitle;
    m_wc.hIconSm = m_wc.hIcon;

    RegisterClassEx(&m_wc);
}

void DisplayWin32::CreateApplicationWindow(int screenWidth, int screenHeight, bool fullscreen)
{
    int posX, posY;

    if (fullscreen)
    {
        DEVMODE dmScreenSettings;
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
        dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

        posX = posY = 0;
    }
    else
    {
        posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
        posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
    }

    m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, m_windowTitle, m_windowTitle,
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
        posX, posY, screenWidth, screenHeight,
        NULL, NULL, m_hInstance, NULL);

    ShowWindow(m_hWnd, SW_SHOW);
    SetForegroundWindow(m_hWnd);
    SetFocus(m_hWnd);
    ShowCursor(false);
}

void DisplayWin32::Shutdown()
{
    ShowCursor(true);

    if (m_hWnd)
    {
        DestroyWindow(m_hWnd);
        m_hWnd = NULL;
    }

    UnregisterClass(m_windowTitle, m_hInstance);
}

//LRESULT CALLBACK DisplayWin32::WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
//{
//    if (s_GameInstance) // Передаем управление обработчику Game
//    {
//        switch (umessage)
//        {
//        case WM_DESTROY:
//        {
//            PostQuitMessage(0);
//            return 0;
//        }
//
//        case WM_CLOSE:
//        {
//            PostQuitMessage(0);
//            return 0;
//        }
//
//        default:
//        {
//            return 0;//ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
//        }
//    }
//}
