#include "pch.h"
#include "CameraScript.h"
#include "Transform.h"
#include "Utils.h"
#include "magic_enum/magic_enum.hpp"

void CameraScript::LateUpdate()
{
    static bool hasTriedFind = false;

    if (_target == nullptr && !hasTriedFind)
    {
        hasTriedFind = true;

        auto& objects = GET_SINGLE(SceneManager)->GetCurrentScene()->GetObjects();
        for (auto& obj : objects)
        {
            if (obj->GetName() == L"Player")
            {
                _target = obj;
                break;
            }
        }
    }

    if (_target == nullptr) return;

    // F8
    if (INPUT->GetButtonDown(KEY_TYPE::F8))
    {
        if (_mode == CameraMode::Follow)
        {
            _mode = CameraMode::Free;
            _lastMousePos = Vec3(INPUT->GetMousePos().x, INPUT->GetMousePos().y, 0);
        }
        else
        {
            _mode = CameraMode::Follow;
        }
    }

    // 모드별 실행
    if (_mode == CameraMode::Follow)
    {
        UpdateFollowMode();
    }
    else if (_mode == CameraMode::Free)
    {
        UpdateFreeMode();
    }

    string msg = "Camera Mode  : " + string(magic_enum::enum_name(_mode));
    LOG_INFO(msg, 2);

}

void CameraScript::UpdateFollowMode()
{
    auto target = _target;
    if (target == nullptr) return;

    Vec3 targetPos = target->GetTransform()->GetPosition();
    Vec3 finalPos = targetPos + _offset;

    GetTransform()->SetPosition(finalPos);
    GetTransform()->LookAt(targetPos);
}

void CameraScript::UpdateFreeMode()
{
    float dt = TIME->GetDeltaTime();

    Vec3 pos = GetTransform()->GetPosition();

    // WASD 이동
    float speed = _speed;

    if (INPUT->GetButton(KEY_TYPE::SHIFT)) speed *= 2.f;
    if (INPUT->GetButton(KEY_TYPE::W)) pos += GetTransform()->GetLook() * speed * dt;
    if (INPUT->GetButton(KEY_TYPE::S)) pos -= GetTransform()->GetLook() * speed * dt;
    if (INPUT->GetButton(KEY_TYPE::A)) pos -= GetTransform()->GetRight() * speed * dt;
    if (INPUT->GetButton(KEY_TYPE::D)) pos += GetTransform()->GetRight() * speed * dt;
    if (INPUT->GetButton(KEY_TYPE::Q)) pos += GetTransform()->GetUp() * speed * dt;
    if (INPUT->GetButton(KEY_TYPE::E)) pos -= GetTransform()->GetUp() * speed * dt;

    GetTransform()->SetPosition(pos);

    // 마우스 우클릭 회전
    if (INPUT->GetButton(KEY_TYPE::RBUTTON))
    {
        const POINT& currentMouse = INPUT->GetMousePos();
        float deltaX = (float)(currentMouse.x - _lastMousePos.x) * 0.5f;
        float deltaY = (float)(currentMouse.y - _lastMousePos.y) * 0.5f;
        Vec3 rot = GetTransform()->GetLocalRotation();
        rot.y += deltaX * dt; 
        rot.x += deltaY * dt; 
        GetTransform()->SetLocalRotation(rot);

        _lastMousePos = Vec3(currentMouse.x, currentMouse.y, 0);
    }
    else
    {
        _lastMousePos = Vec3(INPUT->GetMousePos().x, INPUT->GetMousePos().y, 0);
    }
}

