#pragma once

#include "GameObject.h"

class MainCamera : public GameObject
{
public:
    MainCamera();
    virtual ~MainCamera();

    virtual void Start() override;

    void SetTarget(shared_ptr<GameObject> target) { _target = target; }

private:
    shared_ptr<GameObject> _target;

};

