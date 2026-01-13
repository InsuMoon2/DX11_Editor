#include "pch.h"
#include "EditorManager.h"
#include "ContentBrowserView.h"
#include "GameView.h"
#include "SceneView.h"
#include "HierarchyView.h"
#include "InspectorView.h"
#include "AnimationView.h"
#include "ConsoleView.h"
#include "SceneSerializer.h"

void EditorManager::Init()
{
    // ─────────────────────────────────────────────
    // 에디터 윈도우들 생성 및 등록
    // ─────────────────────────────────────────────
    AddWindow(L"Scene", make_shared<SceneView>());
    //AddWindow(L"Game", make_shared<GameView>());
    AddWindow(L"Hierarchy", make_shared<HierarchyView>());
    AddWindow(L"Inspector", make_shared<InspectorView>());
    AddWindow(L"ContentBrowser", make_shared<ContentBrowserView>());
    AddWindow(L"Animation", make_shared<AnimationView>());
    AddWindow(L"Console", make_shared<ConsoleView>());

}

void EditorManager::Update()
{
    for (auto& [key, window] : _editorWindows)
    {
        if (window && window->IsActive())
            window->Update();
    }
}

void EditorManager::OnGui()
{
    ShowMenuBar();
    BeginDockSpace();

    for (auto& [key, window] : _editorWindows)
    {
        if (window && window->IsActive())
            window->OnGui();
    }

    EndDockSpace();
}

void EditorManager::Release()
{
    _editorWindows.clear();
}

void EditorManager::AddWindow(const wstring& key, shared_ptr<EditorWindow> window)
{
    _editorWindows[key] = window;
    if (window)
        window->Init();
}

shared_ptr<EditorWindow> EditorManager::GetWindow(const wstring& key)
{
    auto it = _editorWindows.find(key);
    if (it != _editorWindows.end())
        return it->second;

    return nullptr;
}

void EditorManager::BeginDockSpace()
{
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
}

void EditorManager::EndDockSpace()
{
    //ImGui::End();
}

void EditorManager::ShowMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            // === Save/Load 추가 ===
            if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
            {
                auto hierarchy = dynamic_pointer_cast<HierarchyView>(GetWindow(L"Hierarchy"));
                if (hierarchy)
                {
                    SceneSerializer::SaveScene(L"../Scenes/Scene.json", hierarchy->GetSceneObjects());
                    LOG_INFO("Scene Saved!");
                }
            }

            if (ImGui::MenuItem("Load Scene", "Ctrl+O"))
            {
                auto hierarchy = dynamic_pointer_cast<HierarchyView>(GetWindow(L"Hierarchy"));
                if (hierarchy)
                {
                    auto objects = SceneSerializer::LoadScene(L"../Scenes/Scene.json");
                    hierarchy->ClearSceneObjects();  // 기존 오브젝트 삭제
                    for (auto& obj : objects)
                        hierarchy->AddObject(obj);
                    LOG_INFO("Scene Loaded!");
                }
            }

            if (ImGui::MenuItem("Exit"))
                PostQuitMessage(0);

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Window"))
        {
            for (auto& [key, window] : _editorWindows)
            {
                bool active = window->IsActive();
                if (ImGui::MenuItem(window->GetName().c_str(), nullptr, &active))
                {
                    window->SetActive(active);
                }
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}
