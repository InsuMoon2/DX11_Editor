#include "pch.h"
#include "SkyDemo.h"
#include "GeometryHelper.h"
#include "Camera.h"
#include "GameObject.h"
#include "CameraScript.h"
#include "Mesh.h"
#include "Model.h"
#include "ModelRenderer.h"
#include "ModelAnimator.h"

void SkyDemo::Init()
{
    RESOURCES->Init();
    _shader = make_shared<Shader>(L"18. SkyDemo.fx");

    // Camera
    _camera = make_shared<GameObject>();
    _camera->GetOrAddTransform()->SetPosition(Vec3{ 0.f, 0.f, -5.f });
    _camera->AddComponent(make_shared<Camera>());
    _camera->AddComponent(make_shared<CameraScript>());

    RENDER->Init(_shader);
}

void SkyDemo::Update()
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
        _obj->Update();
    }
}

void SkyDemo::Render()
{
    
}
