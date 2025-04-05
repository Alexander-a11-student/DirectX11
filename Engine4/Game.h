////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GAME_H_
#define _GAME_H_


///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define WIN32_LEAN_AND_MEAN


//////////////
// INCLUDES //
//////////////
#include <windows.h>
#include <cstdio>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "inputdevice.h"
#include "renderManager.h"
#include "Display1.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: SystemClass
////////////////////////////////////////////////////////////////////////////////
class Game
{
public:
	Game();
	Game(const Game&);
	~Game();

	bool Initialize();
	void Shutdown();
	void Run();
	void Exit();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);


private:
	bool Update(HWND hwnd);
	//void InitializeWindows(int&, int&);
	//void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputDevice* m_Input;
	RenderManager* m_RenderManager;
	Display1* m_Display;
};

/////////////
// GLOBALS //
/////////////
static Game* ApplicationHandle = 0;


#endif
