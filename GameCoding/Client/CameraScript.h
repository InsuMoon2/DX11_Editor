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
    CameraMode _mode = CameraMode::Follow;

    shared_ptr<GameObject> _target;
    Vec3 _offset = Vec3(-0.5f, 5.1f, 5.5f);
    
    float _speed = 20.f;
    Vec3 _lastMousePos = {};
};

