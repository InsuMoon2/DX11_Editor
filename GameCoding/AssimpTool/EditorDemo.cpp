#include "pch.h"
#include "EditorDemo.h"
#include "GeometryHelper.h"
#include "Camera.h"
#include "GameObject.h"
#include "CameraScript.h"
#include "Mesh.h"
#include "Model.h"
#include "ModelRenderer.h"
#include "ModelAnimator.h"

void EditorDemo::Init()
{
    RESOURCES->Init();
    _shader = make_shared<Shader>(L"17. EditorDemo.fx");

    // Camera
    _camera = make_shared<GameObject>();
    _camera->GetOrAddTransform()->SetPosition(Vec3{ 0.f, 0.f, -5.f });
    _camera->AddComponent(make_shared<Camera>());
    //_camera->AddComponent(make_shared<CameraScript>());

    CreateKachujin();

    RENDER->Init(_shader);
}

void EditorDemo::Update()
{
    _camera->Update();
    RENDER->Update();

    {
        LightDesc lightDesc;
        lightDesc.ambient = Vec4(0.4f);
        lightDesc.diffuse = Vec4(1.f);
        lightDesc.specular = Vec4(0.f);
        lightDesc.direction = Vec3(1.f, 0.f, 1.f);
        RENDER->PushLightData(lightDesc);
    }

    {
        UpdateInput();
        UpdateCamera();

        _obj->Update();
    }
}

void EditorDemo::Render()
{
    
}

void EditorDemo::CreateKachujin()
{
    shared_ptr<Model> m1 = make_shared<Model>();
    m1->ReadModel(L"Kachujin/Kachujin");
    m1->ReadMaterial(L"Kachujin/Kachujin");
    m1->ReadAnimation(L"Kachujin/Idle");
    m1->ReadAnimation(L"Kachujin/Run");
    m1->ReadAnimation(L"Kachujin/Slash");

    _obj = make_shared<GameObject>();
    _obj->GetOrAddTransform()->SetPosition(Vec3(0, 0, 50));
    _obj->GetOrAddTransform()->SetScale(Vec3(0.04f));
    //_obj->GetOrAddTransform()->SetRotation(Vec3(3.14f / 2.f, 0, 0));

    _obj->AddComponent(make_shared<ModelAnimator>(_shader));
    {
        _obj->GetModelAnimator()->SetModel(m1);
        //_obj->GetModelAnimator()->SetPass(1);
    }
}

void EditorDemo::UpdateInput()
{

    auto animator = _obj->GetModelAnimator();
    auto transform = _obj->GetTransform();

    if (INPUT->GetButtonDown(KEY_TYPE::LBUTTON))
    {
        _isAttacking = true;
        animator->SetNextAnimation(ANIM_SLASH);
    }
    if (_isAttacking)
    {
        if (animator->GetCurrentAnimIndex() == ANIM_SLASH &&
            animator->IsAnimationEnd() &&        
            !animator->IsTransitioning())
        {
            animator->SetNextAnimation(ANIM_IDLE);
        }

        if (animator->IsPlaying(ANIM_IDLE))
        {
            _isAttacking = false;
        }
        return;
    }

    // ========== 이동 처리 ==========
    Vec3 moveDir = Vec3::Zero;
    bool isMoving = false;

    if (INPUT->GetButton(KEY_TYPE::W))
    {
        moveDir.z += 1.f;
        isMoving = true;
    }
    if (INPUT->GetButton(KEY_TYPE::S))
    {
        moveDir.z -= 1.f;
        isMoving = true;
    }
    if (INPUT->GetButton(KEY_TYPE::A))
    {
        moveDir.x -= 1.f;
        isMoving = true;
    }
    if (INPUT->GetButton(KEY_TYPE::D))
    {
        moveDir.x += 1.f;
        isMoving = true;
    }

    if (isMoving)
    {
        // 방향 정규화
        moveDir.Normalize();

        // 위치 업데이트
        Vec3 pos = transform->GetPosition();
        pos += moveDir * _moveSpeed * DT;
        transform->SetPosition(pos);

        // 캐릭터 회전 (이동 방향 바라보기)
        if (moveDir.LengthSquared() > 0)
        {
            float angle = atan2f(moveDir.x, moveDir.z);
            transform->SetRotation(Vec3(0, angle, 0));
        }

        // Run 애니메이션
        if (!animator->IsPlaying(ANIM_RUN))
        {
            animator->SetNextAnimation(ANIM_RUN);
        }
    }
    else
    {
        // Idle 애니메이션
        if (!animator->IsPlaying(ANIM_IDLE))
        {
            animator->SetNextAnimation(ANIM_IDLE);
        }
    }

}

void EditorDemo::UpdateCamera()
{
    Vec3 playerPos = _obj->GetTransform()->GetPosition();

    Vec3 cameraPos = playerPos + Vec3(0, 15.f, 25.f);  
    _camera->GetTransform()->SetPosition(cameraPos);

    Vec3 lookTarget = playerPos + Vec3(0, 5.f, 0);  
    Vec3 lookDir = lookTarget - cameraPos;
    lookDir.Normalize();

    float pitch = asinf(-lookDir.y);  
    float yaw = atan2f(lookDir.x, lookDir.z); 

    _camera->GetTransform()->SetRotation(Vec3(pitch, yaw, 0));
}
