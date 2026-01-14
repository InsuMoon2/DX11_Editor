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
#include "Camera.h"
#include "RenderTarget.h"


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

void EditorManager::Render()
{
    auto sceneView = dynamic_pointer_cast<SceneView>(GetWindow(L"Scene"));

    if (sceneView && sceneView->GetRenderTarget())
    {
        auto renderTarget = sceneView->GetRenderTarget();

        renderTarget->BindAsTarget();
        renderTarget->Clear(Color(0.1f, 0.1f, 0.1f, 1.f));

        // 에디터 카메라
        auto editorCam = sceneView->GetEditorCamera();
        if (editorCam)
        {
            editorCam->GetCamera()->UpdateMatrix();
            RENDER->PushGlobalData(Camera::S_MatView, Camera::S_MatProjection);
        }

        // 조명 설정 (기본값)
        LightDesc lightDesc;
        lightDesc.ambient = Vec4(0.4f);
        lightDesc.diffuse = Vec4(1.f);
        lightDesc.specular = Vec4(0.f);
        lightDesc.direction = Vec3(1.f, 0.f, 1.f);
        RENDER->PushLightData(lightDesc);

        // 씬 렌더링
        if (GET_SINGLE(SceneManager)->IsPlaying()) 
        {
            if (!GET_SINGLE(SceneManager)->IsPaused())
                CUR_SCENE->Update();

            CUR_SCENE->Render();
        }
        else
        {
            auto hierarchy = dynamic_pointer_cast<HierarchyView>(GetWindow(L"Hierarchy"));
            if (hierarchy)
            {
                auto& objects = hierarchy->GetSceneObjects();
                vector<shared_ptr<GameObject>> vec(objects.begin(), objects.end());

                for (auto& obj : vec)
                    if (obj) obj->GetTransform()->UpdateTransform();

                INSTANCING->Render(vec);
            }
        }
        // 에디터 모드: Hierarchy에 있는 애들만 직접 렌더링
        UpdateHierarchy();

        RenderTarget::UnbindAll();
    }

    OnGui();
}

void EditorManager::UpdateHierarchy()
{
    auto hierarchy = dynamic_pointer_cast<HierarchyView>(GetWindow(L"Hierarchy"));
    if (hierarchy)
    {
        auto& sceneObjects = CUR_SCENE->GetObjects();
        auto& viewObjects = hierarchy->GetSceneObjects();
        
        if (sceneObjects.size() == viewObjects.size())
            return;
        
        hierarchy->ClearSceneObjects();
        for (auto& obj : sceneObjects)
        {
            hierarchy->AddObject(obj);
        }
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
