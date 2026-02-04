#pragma once

#include "EditorWindow.h"

class EditorManager
{
    DECLARE_SINGLE(EditorManager)

public:
    void Init();
    void Update();
    void Render();
    void OnGui();
    void Release();

    void AddWindow(const wstring& key, shared_ptr<EditorWindow> window);
    shared_ptr<EditorWindow> GetWindow(const wstring& key);

    void UpdateHierarchy();

    template<typename T>
    shared_ptr<T> GetWindow(const wstring& name)
    {
        return dynamic_pointer_cast<T>(GetWindow(name));
    }

private:
    void BeginDockSpace();
    void EndDockSpace();
    void ShowMenuBar();

private:
    map<wstring, shared_ptr<EditorWindow>> _editorWindows;

    //ImGuiWindowFlags _windowFlags = ImGuiWindowFlags_None;
    //ImGuiDockNodeFlags _dockspaceFlags = ImGuiDockNodeFlags_None;
    //bool _fullScreen = true;

};

