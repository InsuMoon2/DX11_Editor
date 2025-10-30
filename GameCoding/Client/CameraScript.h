#pragma once

#include "MonoBehavior.h"

class CameraScript : public MonoBehavior
{
public:
    CameraScript() = default;
    virtual ~CameraScript() {}

    void Start() override;
    void Update() override;

    float _speed = 10.f;
};

