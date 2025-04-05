#include "inputdevice.h"


InputDevice::InputDevice()
{
	m_mouseWheelDelta = 0; // Инициализация новой переменной
}


InputDevice::InputDevice(const InputDevice& other)
{
}


InputDevice::~InputDevice()
{
}


void InputDevice::Initialize(HWND hwnd)
{
	m_hwnd = hwnd;
	int i;
	for (i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}
	m_mouseButtons[0] = false;
	m_mouseButtons[1] = false;
	m_mouseX = 750; // Центр окна 1500x1500
	m_mouseY = 750;
	m_mouseDeltaX = 0;
	m_mouseDeltaY = 0;
	m_mouseWheelDelta = 0;

	ShowCursor(FALSE); // Скрываем курсор
}


void InputDevice::KeyDown(unsigned int input)
{
	m_keys[input] = true;
	return;
}


void InputDevice::KeyUp(unsigned int input)
{

	m_keys[input] = false;
	return;
}


bool InputDevice::IsKeyDown(unsigned int key)
{
	return m_keys[key];
}

void InputDevice::MouseButtonDown(int button)
{
	m_mouseButtons[button] = true;
	return;
}

void InputDevice::MouseButtonUp(int button)
{
	m_mouseButtons[button] = false;
	return;
}

void InputDevice::SetMousePosition(int x, int y)
{
	m_mouseX = x;
	m_mouseY = y;
	return;
}

void InputDevice::GetMousePosition(int& x, int& y)
{
	x = m_mouseX;
	y = m_mouseY;
	return;
}

void InputDevice::UpdateMouseDelta(int x, int y)
{
	m_mouseDeltaX = x - 750;
	m_mouseDeltaY = y - 750;

	POINT center = { 750, 750 };
	ClientToScreen(m_hwnd, &center);
	SetCursorPos(center.x, center.y);
}

void InputDevice::GetMouseDelta(int& deltaX, int& deltaY)
{
	deltaX = m_mouseDeltaX;
	deltaY = m_mouseDeltaY;
	m_mouseDeltaX = 0;
	m_mouseDeltaY = 0;
}

void InputDevice::UpdateMouseWheel(int delta)
{
	m_mouseWheelDelta = delta; // Сохраняем дельту прокрутки
}

int InputDevice::GetMouseWheelDelta()
{
	int delta = m_mouseWheelDelta;
	m_mouseWheelDelta = 0; // Сбрасываем после чтения
	return delta;
}