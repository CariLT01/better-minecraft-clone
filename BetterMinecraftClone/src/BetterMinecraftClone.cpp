// BetterMinecraftClone.cpp : Defines the entry point for the application.
//

#ifndef NDEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#define new new(_NORMAL_BLOCK, __FILE__, __LINE__) 
#endif

#include "BetterMinecraftClone.h"
#include "Game.h"

int main()
{

#ifndef NDEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);

	// _CrtSetBreakAlloc(977);

	// int* testLeak = new int[100];
#endif

	Game* game = new Game();
	// g    
	game->run();


	delete game;

#ifndef NDEBUG
	_CrtDumpMemoryLeaks();
#endif
	return 0;
}
