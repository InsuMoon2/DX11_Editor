#include "pch.h"
#include "Stage01.h"
#include "Camera.h"
#include "CameraScript.h"
#include "Light.h"
#include "Player.h"
#include "FollowCamera.h"
#include "MainCamera.h"

Stage01::Stage01()
{
}

Stage01::~Stage01()
{
}

void Stage01::Init()
{
    // Light
    {
        auto light = make_shared<GameObject>();
        light->GetOrAddTransform();
        light->AddComponent(make_shared<Light>());
        LightDesc lightDesc;
        lightDesc.ambient   = Vec4(0.4f);
        lightDesc.diffuse   = Vec4(1.f);
        lightDesc.specular  = Vec4(0.1f);
        lightDesc.direction = Vec3(1.f, 0.f, 1.f);
        light->GetLight()->SetLightDesc(lightDesc);
        light->SetName(L"Light");

        Add_Scene(light);
    }

    // Player
    {
        _player = make_shared<Player>();
        _player->GetOrAddTransform()->SetPosition(Vec3(0, 0, 0));
        Add_Scene(_player);
    }

    // Camera
    {
        auto mainCamera = make_shared<MainCamera>();
        mainCamera->SetTarget(_player);

        Add_Scene(mainCamera);
    }
}

void Stage01::Update()
{
    Scene::Update();

    
}
