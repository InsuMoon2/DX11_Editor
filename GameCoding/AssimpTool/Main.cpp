#include "pch.h"
#include "Main.h"
#include "Engine/Game.h"
#include "AssimpTool.h"
#include "StaticMeshDemo.h"
#include "ImGuiDemo.h"
#include "AnimationDemo.h"
#include "TweenDemo.h"
#include "SkyDemo.h"
#include "EditorDemo.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	GameDesc desc;
	desc.appName = L"GameCoding";
	desc.hInstance = hInstance;
	desc.vsync = false;
	desc.hWnd = NULL;
	desc.width = 1280;
	desc.height = 720;
	desc.clearColor = Color(0.2f, 0.2f, 0.2f, 0.2f);

	desc.app = make_shared<AssimpTool>();
	//desc.app = make_shared<EditorDemo>();

	GAME->Run(desc);

	return 0;
}
