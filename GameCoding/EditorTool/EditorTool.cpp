#include "pch.h"
#include "Engine/Game.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    GameDesc desc;
    desc.appName = L"EditorTool";
    desc.hInstance = hInstance;
    desc.vsync = false;
    desc.hWnd = NULL;
    desc.width = 1600;   
    desc.height = 900;
    desc.clearColor = Color(0.2f, 0.2f, 0.2f, 1.0f);

    GAME->Run(desc);

    return 0;
}

