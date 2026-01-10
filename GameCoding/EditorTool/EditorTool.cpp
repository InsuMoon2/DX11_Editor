#include "pch.h"
#include "EditorToolApp.h"
#include "Engine/Game.h"

#include <ShellScalingApi.h>
#pragma comment(lib, "Shcore.lib")

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

    GameDesc desc;
    desc.appName = L"EditorTool";
    desc.hInstance = hInstance;
    desc.vsync = false;
    desc.hWnd = NULL;
    desc.width = 1600;   
    desc.height = 900;
    desc.clearColor = Color(0.2f, 0.2f, 0.2f, 1.0f);
    desc.app = make_shared<EditorToolApp>();
    GAME->Run(desc);
    return 0;
}

