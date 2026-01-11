#pragma once

class ModelAnimator;

class IAnimNotify
{
public:
    IAnimNotify() = default;
    virtual ~IAnimNotify() = default;

    virtual wstring GetName() const = 0;
    virtual void OnNotify(ModelAnimator* animator) = 0;
};

