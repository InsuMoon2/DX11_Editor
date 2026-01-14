#include "pch.h"
#include "MainCamera.h"
#include "Camera.h"
#include "CameraScript.h"

MainCamera::MainCamera()
{
    SetName(L"MainCamera");
}

MainCamera::~MainCamera()
{
}

void MainCamera::Start()
{
    GameObject::Start();

    GetOrAddTransform();

    AddComponent(make_shared<Camera>());

    auto script = make_shared<CameraScript>();
    script->SetTarget(_target);

    AddComponent(script);
}
