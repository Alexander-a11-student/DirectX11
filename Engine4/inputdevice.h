#ifndef _INPUTDEVICE_H_
#define _INPUTDEVICE_H_


////////////////////////////////////////////////////////////////////////////////
// Class name: InputClass
////////////////////////////////////////////////////////////////////////////////
#include <windowsx.h>
#include <windows.h> // ��������� ��� HWND � ������� Windows API


class InputDevice
{
public:
	InputDevice();
	InputDevice(const InputDevice&);
	~InputDevice();

	void Initialize(HWND hwnd);

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);
	bool IsKeyDown(unsigned int);

	void MouseButtonDown(int button);
	void MouseButtonUp(int button);

	void SetMousePosition(int x, int y);
	void GetMousePosition(int& x, int& y);

	void UpdateMouseDelta(int x, int y);
	void GetMouseDelta(int& deltaX, int& deltaY);

	// ����� ������ ��� ��������
	void UpdateMouseWheel(int);
	int GetMouseWheelDelta();

	int m_mouseDeltaX = 0; 
	int m_mouseDeltaY = 0; // ������� �� ����

private:
	HWND m_hwnd;
	bool m_keys[256];
	bool m_mouseButtons[2]; 
	int m_mouseX, m_mouseY;
	int m_mouseWheelDelta;

};

#endif
