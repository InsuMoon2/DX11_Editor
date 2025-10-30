#pragma once

#include "Component.h"

class MonoBehavior : public Component
{
    using Super = Component;

public:
    MonoBehavior();
    virtual ~MonoBehavior();

    virtual void Awake() override;
    virtual void Update() override;
};



