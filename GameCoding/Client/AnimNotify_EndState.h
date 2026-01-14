#pragma once

#include "AnimNotify.h"
#include "StateComponent.h"

class AnimNotify_EndState : public AnimNotify
{
public:
    AnimNotify_EndState() = default;
    ~AnimNotify_EndState() override = default;

public:
    wstring GetDisplayName() const override;
    void OnNotify(ModelAnimator* animator) override;
    void OnGui() override;

private:
    CharacterState _state = CharacterState::Idle;

};

