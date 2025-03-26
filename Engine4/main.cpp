////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Game.h"
#include <iostream>
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Game* game;
	bool result;

	game = new Game;

	result = game->Initialize();
	if (result)
	{
		game->Run();
	}

	game->Shutdown();
	delete game;
	game = 0;


	return 0;
}

