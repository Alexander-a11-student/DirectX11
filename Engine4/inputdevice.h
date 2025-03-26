#ifndef _INPUTDEVICE_H_
#define _INPUTDEVICE_H_


////////////////////////////////////////////////////////////////////////////////
// Class name: InputClass
////////////////////////////////////////////////////////////////////////////////
#include <windowsx.h>

class InputDevice
{
public:
	InputDevice();
	InputDevice(const InputDevice&);
	~InputDevice();

	void Initialize();

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);
	bool IsKeyDown(unsigned int);

	void MouseButtonDown(int button);
	void MouseButtonUp(int button);

	void SetMousePosition(int x, int y);
	void GetMousePosition(int& x, int& y);

	void UpdateMouseDelta(int x, int y);
	void GetMouseDelta(int& deltaX, int& deltaY);

	int m_mouseDeltaX = 0; 
	int m_mouseDeltaY = 0; // Разница по осям

private:
	bool m_keys[256];
	bool m_mouseButtons[2]; // 0 - левая кнопка, 1 - правая кнопка
	int m_mouseX, m_mouseY;

};

#endif
