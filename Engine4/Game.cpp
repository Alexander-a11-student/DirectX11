////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Game.h"



Game::Game()
{
	m_Input = 0;
	m_RenderManager = 0;
	m_Display = 0;
}


Game::Game(const Game& other)
{
}


Game::~Game()
{
}


bool Game::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	screenWidth = 1500;
	screenHeight = 1500;

	m_Input = new InputDevice;
	m_Display = new Display1();

	m_Display->Initialize(this, screenWidth, screenHeight, false);
	m_hwnd = m_Display->GetHwnd();
	m_Input->Initialize(m_hwnd);
	

	m_RenderManager = new RenderManager;
	result = m_RenderManager->Initialize(screenWidth, screenHeight, m_hwnd); //Вот это важно!!!
	if (!result)
	{
		return false;
	}

	return true;
}

void Game::Exit()
{
	Shutdown();
	PostQuitMessage(0);  
}


void Game::Shutdown()
{
	// Сбрасываем ограничение курсора мыши
	ClipCursor(nullptr);

	if (m_RenderManager)
	{
		m_RenderManager->Shutdown();
		delete m_RenderManager;
		m_RenderManager = 0;
	}

	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	if (m_Display) {
		m_Display->Shutdown();
		delete m_Display;
		m_Display = 0;
	}

	return;
}

void Game::Run()
{
	MSG msg;
	bool done, result;


	ZeroMemory(&msg, sizeof(MSG));

	done = false;
	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			done = true; 
		}
		else
		{
			result = Update(m_hwnd);
			if (!result)
			{
				done = true;
			}
		}
	}
	return;
}





bool Game::Update(HWND hwnd)
{
	bool result;


	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		Exit();
		return false;
	}

	result = m_RenderManager->Update(hwnd);
	if (!result)
	{
		return false;
	}

	return true;
}


LRESULT CALLBACK Game::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
	case WM_KEYDOWN:
	{

		m_Input->KeyDown((unsigned int)wparam);
		// Движение камеры к планетам:
		if (m_Input->IsKeyDown(0x57)) { // W
			m_RenderManager->MoveBarrelForward();
		}
		if (m_Input->IsKeyDown(0x53)) { // S
			m_RenderManager->MoveBarrelBackward();
		}
		if (m_Input->IsKeyDown(0x41)) { // A
			m_RenderManager->MoveBarrelLeft();
		}
		if (m_Input->IsKeyDown(0x44)) { // D
			m_RenderManager->MoveBarrelRight();
		}

		if (m_Input->IsKeyDown(0xDB)) { // [ (Квадратная скобка влево)
			m_RenderManager->TurnLeft();
		}
		if (m_Input->IsKeyDown(0xDD)) { // ] (Квадратная скобка вправо)
			m_RenderManager->TurnRight();
		}
		// Вращение камеры по оси X с помощью Page Up и Page Down
		if (m_RenderManager && m_Input->IsKeyDown(VK_PRIOR)) { // Page Up
			m_RenderManager->RotateCameraUp();
		}
		if (m_RenderManager && m_Input->IsKeyDown(VK_NEXT)) { // Page Down
			m_RenderManager->RotateCameraDown();
		}

		return 0;
	}

	case WM_KEYUP:
		{
			m_Input->KeyUp((unsigned int)wparam);
			return 0;
		}

	case WM_LBUTTONDOWN: 
		{
			m_Input->MouseButtonDown(0);
			OutputDebugStringA("Левая кнопка  активна!\n");
			return 0;
		}


	case WM_LBUTTONUP:
		{
			m_Input->MouseButtonUp(0);
			return 0;
		}


	case WM_RBUTTONDOWN:
		{
			m_Input->MouseButtonDown(1);
			OutputDebugStringA("Правая кнопка активна!\n");
			return 0;
		}



	case WM_RBUTTONUP:
		{
			m_Input->MouseButtonUp(1);
			return 0;
		}


	case WM_MOUSEMOVE:
	{
		int mouseX = GET_X_LPARAM(lparam);
		int mouseY = GET_Y_LPARAM(lparam);

		if (m_Input)
		{
			m_Input->UpdateMouseDelta(mouseX, mouseY);
		}

		int mouseXDelta = m_Input->m_mouseDeltaX;
		int mouseYDelta = m_Input->m_mouseDeltaY;



		if (m_RenderManager)
		{
			m_RenderManager->UpdateMouseMovement(mouseXDelta, mouseYDelta);
		}

		return 0;
	}

	// Добавляем обработку колесика
	case WM_MOUSEWHEEL:
	{
		int wheelDelta = GET_WHEEL_DELTA_WPARAM(wparam); // Получаем дельту прокрутки
		if (m_Input)
		{
			m_Input->UpdateMouseWheel(wheelDelta);
		}

		int wheelDeltaValue = m_Input->GetMouseWheelDelta();
		if (m_RenderManager)
		{
			m_RenderManager->UpdateMouseWheel(wheelDeltaValue); // Передаём дельту в RenderManager
		}
		return 0;
	}

	default:
	{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	}
}