#include <sqrat.h>
#include <sqstdmath.h>

#include <string>
#include <iostream>

#include "Game.h"
#include "CSquirrel.h"
#include "DebugConsole.h"

using namespace Kotone;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);

	auto game = new Game;
	game->Init();

#if _DEBUG
	// デバッグ用コンソールを用意する
	auto console = new DebugConsole;
	console->setConsole();
#endif

	auto sq = new CSquirrel;
	sq->Init();

	// ゲームのメインループ
	game->Run();

	/* 後始末 */
	game->Finalize();
	sq->Finalize();
#if _DEBUG
	console->Finalize();
	delete console;
#endif

	delete sq;
	delete game;

	return 0;
}
