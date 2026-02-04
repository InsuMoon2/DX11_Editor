#pragma once

class ModelAnimator;

class IAnimNotifyState
{
public:
    IAnimNotifyState() = default;
    virtual ~IAnimNotifyState() = default;

    virtual wstring GetName() const = 0;

    virtual void OnNotifyBegin(ModelAnimator* animator) = 0;
    virtual void OnNotifyTick(ModelAnimator* animator, float deltaTime) = 0;
    virtual void OnNotifyEnd(ModelAnimator* animator) = 0;
};

