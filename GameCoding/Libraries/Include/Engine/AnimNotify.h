#pragma once

class ModelAnimator;

class AnimNotify
{
public:
    AnimNotify() = default;
    virtual ~AnimNotify() = default;

    virtual wstring GetDisplayName() const = 0;
    virtual void OnNotify(ModelAnimator* animator) = 0;
    virtual void OnGui();

public:
    int  GetFrame() { return _frame; }
    void SetFrame(int frame) { _frame = frame; }

protected:
    // 공통 데이터
    int _frame = 0;
};

