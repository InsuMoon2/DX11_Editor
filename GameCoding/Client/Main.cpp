#include "pch.h"
#include "Main.h"
#include "Engine/Game.h"
#include "SceneDemo.h"
#include "GameDemo.h"
#include "Stage01.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	GameDesc desc;
	desc.appName = L"GameCoding";
	desc.hInstance = hInstance;
	desc.vsync = false;
	desc.hWnd = NULL;
	desc.width = 1600;
	desc.height = 900;
	desc.clearColor = Color(0.f, 0.f, 0.f, 0.f);
	desc.app = make_shared<GameDemo>();

	GAME->Run(desc);

	return 0;
}
