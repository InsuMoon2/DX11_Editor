#pragma once

class ModelAnimator;

class AnimNotifyState
{
public:
    AnimNotifyState() = default;
    virtual ~AnimNotifyState() = default;

    virtual wstring GetDisplayName() const = 0;

    virtual void OnNotifyBegin(ModelAnimator* animator) = 0;
    virtual void OnNotifyTick(ModelAnimator* animator, float deltaTime) = 0;
    virtual void OnNotifyEnd(ModelAnimator* animator) = 0;

public:
    int  GetStartFrame() { return _startFrame; }
    int  GetEndFrame() { return _endFrame; }

    void SetStartFrame(int frame) { _startFrame = frame; }
    void SetEndFrame(int frame) { _endFrame = frame; }

protected:
    int _startFrame = 0;
    int _endFrame = 0;

};

