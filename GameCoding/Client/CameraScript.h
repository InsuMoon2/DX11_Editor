#pragma once

#include "MonoBehavior.h"

enum class CameraMode
{
    Follow, Free, End
};

class CameraScript : public MonoBehavior
{
public:
    virtual void LateUpdate() override;

    void SetTarget(shared_ptr<GameObject> target) { _target = target; }
    void SetOffset(Vec3 offset) { _offset = offset; }

private:
    void UpdateFollowMode();
    void UpdateFreeMode();

private:
    shared_ptr<GameObject> _target;
    CameraMode _mode = CameraMode::Follow;

    Vec3    _offset = Vec3(-0.5f, 5.1f, 5.5f);
    Vec3    _lastMousePos = {};
    
    float   _speed = 20.f;

private:
    // 카메라 회전
    float _yaw = 0.f;
    float _pitch = 0.f;
    float _distance = 5.f;
    float _sensivity = 0.003f;


};

