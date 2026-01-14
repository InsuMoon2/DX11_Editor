#pragma once

#include "MonoBehavior.h"

class FollowCamera : public MonoBehavior
{
public:
    virtual void LateUpdate() override;

    void SetTarget(shared_ptr<GameObject> target) { _target = target; }
    void SetOffset(Vec3 offset) { _offset = offset; }

private:
    shared_ptr<GameObject> _target; 
    Vec3 _offset = Vec3(0.f, 3.f, 5.f);
    float _smoothSpeed = 5.0f;
};

