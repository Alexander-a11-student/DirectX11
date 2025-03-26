#include "inputdevice.h"


InputDevice::InputDevice()
{
}


InputDevice::InputDevice(const InputDevice& other)
{
}


InputDevice::~InputDevice()
{
}


void InputDevice::Initialize()
{
	int i;

	for (i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}
	m_mouseButtons[0] = false;
	m_mouseButtons[1] = false;

	m_mouseX = 0;
	m_mouseY = 0;

	return;
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
	m_mouseDeltaX = x - m_mouseX;
	m_mouseDeltaY = y - m_mouseY;

	m_mouseX = x;
	m_mouseY = y;
}

void InputDevice::GetMouseDelta(int& deltaX, int& deltaY)
{
	deltaX = m_mouseDeltaX;
	deltaY = m_mouseDeltaY;

	// Сбрасываем после чтения, чтобы не накапливались значения
	m_mouseDeltaX = 0;
	m_mouseDeltaY = 0;
}