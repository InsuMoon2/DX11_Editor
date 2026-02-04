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

    // F8 -> FreeMode
    if (INPUT->GetButtonDown(KEY_TYPE::F8))
    {
        if (_mode == CameraMode::Follow)
        {
            _mode = CameraMode::Free;
            INPUT->UnlockMouse();
            _lastMousePos = Vec3(INPUT->GetMousePos().x, INPUT->GetMousePos().y, 0);
        }
        else
        {
            _mode = CameraMode::Follow;
            INPUT->LockMouse();
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

    // 마우스 잠금일 때에만, 카메라 회전
    if (INPUT->IsMouseLocked())
    {
        Vec2 mouseDelta = INPUT->GetMouseDelta();

        _yaw += mouseDelta.x * _sensivity;
        _pitch += mouseDelta.y * _sensivity;

        // Pitch Clamp : 일단은 45도로 제한
        _pitch = std::clamp(_pitch, XMConvertToRadians(-45.f), XMConvertToRadians(70.f));
    }

    // 카메라 위치 계산
    Vec3 targetPos = target->GetTransform()->GetPosition();

    // 플레이어 뒤쪽 위치 계산
    float camX = sinf(_yaw) * cosf(_pitch) * _distance;
    float camY = sinf(_pitch) * _distance + 2.f; 
    float camZ = cosf(_yaw) * cosf(_pitch) * _distance;

    Vec3 cameraPos = targetPos + Vec3(camX, camY, camZ);

    GetTransform()->SetPosition(cameraPos);
    GetTransform()->LookAt(targetPos + Vec3(0, 1.5f, 0));
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

