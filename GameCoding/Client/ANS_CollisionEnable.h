#pragma once

#include "AnimNotifyState.h"

class ANS_CollisionEnable : public AnimNotifyState
{
public:
    ANS_CollisionEnable();
    ~ANS_CollisionEnable() override = default;

public:
    wstring GetDisplayName() const override;

    void OnNotifyBegin(ModelAnimator* animator) override;
    void OnNotifyTick(ModelAnimator* animator, float deltaTime) override;
    void OnNotifyEnd(ModelAnimator* animator) override;

private:
    int _testInt = 2;
};

