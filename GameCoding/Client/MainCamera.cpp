#include "pch.h"
#include "MainCamera.h"
#include "Camera.h"
#include "CameraScript.h"

REGISTER_GAMEOBJECT(MainCamera)

MainCamera::MainCamera()
{
    SetName(L"MainCamera");
}

MainCamera::~MainCamera()
{
}

void MainCamera::Awake()
{
    GameObject::Awake();

    GetOrAddTransform();

    AddComponent(make_shared<Camera>());

    auto script = make_shared<CameraScript>();
    script->SetTarget(_target);

    AddComponent(script);
}

