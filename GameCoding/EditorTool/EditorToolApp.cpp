#include "pch.h"
#include "EditorToolApp.h"
#include "HierarchyView.h"
#include "Camera.h"
#include "Model.h"
#include "EditorManager.h"
#include "SceneView.h"
#include "Engine/RenderTarget.h"
#include "ModelAnimator.h"

void EditorToolApp::Init()
{
    RESOURCES->Init();
    _shader = make_shared<Shader>(L"17. TweenDemo.fx"); 

    RENDER->Init(_shader);

    CreatePlayer();

    GET_SINGLE(EditorManager)->Init();

    RegisterHierarchy();

}

void EditorToolApp::Update()
{
    GET_SINGLE(EditorManager)->Update();
}

void EditorToolApp::Render()
{
    // ─────────────────────────────────────────────
    // 1. Scene View의 RenderTarget에 씬 렌더링
    // ─────────────────────────────────────────────
    auto sceneView = dynamic_pointer_cast<SceneView>(
        GET_SINGLE(EditorManager)->GetWindow(L"Scene"));

    if (sceneView && sceneView->GetRenderTarget())
    {
        auto renderTarget = sceneView->GetRenderTarget();

        // RenderTarget 바인딩
        renderTarget->BindAsTarget();
        renderTarget->Clear(Color(0.1f, 0.1f, 0.1f, 1.f));

        // 에디터 카메라 뷰/프로젝션 설정
        auto editorCamera = sceneView->GetEditorCamera();
        if (editorCamera)
        {
            editorCamera->GetCamera()->UpdateMatrix();

            auto camera = editorCamera->GetCamera();
            RENDER->PushGlobalData(Camera::S_MatView, Camera::S_MatProjection);
        }

        // ─────────────────────────────────────────────
        // TODO: 여기에 씬의 게임 오브젝트들 렌더링
        // 예: for (auto& obj : _sceneObjects) obj->Render();
        // ─────────────────────────────────────────────
        LightDesc lightDesc;
        lightDesc.ambient = Vec4(0.4f);
        lightDesc.diffuse = Vec4(1.f);
        lightDesc.specular = Vec4(0.f);
        lightDesc.direction = Vec3(1.f, 0.f, 1.f);
        RENDER->PushLightData(lightDesc);

        // ─────────────────────────────────────────────
        // 플레이어 렌더링
        // ─────────────────────────────────────────────
        if (_player)
            _player->Update();

        // RenderTarget 해제 (백버퍼로 복원은 Graphics에서)
        RenderTarget::UnbindAll();
    }

    // ─────────────────────────────────────────────
    // 2. ImGui (에디터 UI) 렌더링
    // ─────────────────────────────────────────────
    GET_SINGLE(EditorManager)->OnGui();
}

void EditorToolApp::CreatePlayer()
{
    shared_ptr<Model> model = make_shared<Model>();
    model->ReadModel(L"Kachujin/Kachujin");
    model->ReadMaterial(L"Kachujin/Kachujin");
    model->ReadAnimation(L"Kachujin/Idle");

    _player = make_shared<GameObject>();
    _player->GetOrAddTransform()->SetPosition(Vec3(0, 0, 10));
    _player->GetOrAddTransform()->SetScale(Vec3(0.04f));

    _player->AddComponent(make_shared<ModelAnimator>(_shader));
    _player->GetModelAnimator()->SetModel(model);

}

void EditorToolApp::RegisterHierarchy()
{
    auto hierarchy = dynamic_pointer_cast<HierarchyView>(
        GET_SINGLE(EditorManager)->GetWindow(L"Hierarchy"));

    if (hierarchy)
    {
        hierarchy->AddObject(_player);
    }
}
