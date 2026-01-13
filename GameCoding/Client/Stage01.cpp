#include "pch.h"
#include "Stage01.h"
#include "Camera.h"
#include "Light.h"
#include "Player.h"
#include "CameraScript.h"

Stage01::Stage01()
{
}

Stage01::~Stage01()
{
}

void Stage01::Init(shared_ptr<Shader> shader)
{
    // Camera
    {
        auto camera = make_shared<GameObject>();
        camera->GetOrAddTransform()->SetPosition(Vec3(0.f, 5.f, -10.f));
        camera->AddComponent(make_shared<Camera>());
        camera->AddComponent(make_shared<CameraScript>());
    }

    // Light
    {
        auto light = make_shared<GameObject>();
        light->AddComponent(make_shared<Light>());
        LightDesc lightDesc;
        lightDesc.ambient = Vec4(0.4f);
        lightDesc.diffuse = Vec4(1.f);
        lightDesc.specular = Vec4(0.1f);
        lightDesc.direction = Vec3(1.f, 0.f, 1.f);
        light->GetLight()->SetLightDesc(lightDesc);

        Add(light);
    }

    // Player
    {
        _player = make_shared<Player>();
        _player->Init(shader);
        _player->GetOrAddTransform()->SetPosition(Vec3(0, 0, 0));
        Add(_player);
    }
}

