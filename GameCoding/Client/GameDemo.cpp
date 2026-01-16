#include "pch.h"
#include "GameDemo.h"
#include "Camera.h"
#include "GameObject.h"
#include "CameraScript.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "Model.h"
#include "ModelRenderer.h"
#include "ModelAnimator.h"
#include "Light.h"
#include "Stage01.h"
#include "EditorManager.h"
#include "Player.h"
#include "ContentBrowserView.h"
#include "Model.h"
#include "FileUtils.h"
#include "SceneSerializer.h"

void GameDemo::Init()
{
    RESOURCES->Init(); 
    _shader = make_shared<Shader>(L"23. RenderDemo.fx");
    RENDER->Init(_shader);

    GET_SINGLE(AnimNotifyManager)->LoadAllFromJson(L"../Resources/Notifies/");

    _startScene = make_shared<Stage01>();
    SCENE->ChangeScene(_startScene);

    GET_SINGLE(EditorManager)->Init();

    auto stage = dynamic_pointer_cast<Stage01>(_startScene);
    if (stage)
    {
        auto player = stage->GetPlayer();
        auto model = player->GetModelAnimator()->GetModel(); // Player에서 Model 가져오기

        // 애니메이션 경로 하드코딩 (또는 Player에 저장된 것 사용)
        vector<wstring> animPaths = {
            L"Kachujin/Idle",
            L"Kachujin/Run",
            L"Kachujin/Slash"
        };

        auto contentBrowser = dynamic_pointer_cast<ContentBrowserView>(
            GET_SINGLE(EditorManager)->GetWindow(L"ContentBrowser"));
        if (contentBrowser && model)
        {
            auto& animations = model->GetAnimations();
            for (int i = 0; i < animations.size(); i++)
            {
                wstring name = FileUtils::PathToAnimName(animPaths[i]); // FileUtils 필요
                contentBrowser->AddAnimation(
                    name,
                    model,
                    player->GetModelAnimator(),
                    animPaths,
                    i
                );
            }
        }
    }

    // 카메라 가두기
    INPUT->LockMouse();

}

void GameDemo::Update()
{
    GET_SINGLE(EditorManager)->Update();

    CUR_SCENE->Update();

}

void GameDemo::Render()
{
    GET_SINGLE(EditorManager)->Render();

    //CUR_SCENE->Render();
}
