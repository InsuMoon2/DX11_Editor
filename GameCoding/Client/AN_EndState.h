#pragma once

#include "AnimNotify.h"
#include "StateComponent.h"

class AN_EndState : public AnimNotify
{
public:
    AN_EndState();
    ~AN_EndState() override = default;

public:
    wstring GetDisplayName() const override;
    void OnNotify(ModelAnimator* animator) override;
    void OnGui() override;

private:
    CharacterState _state = CharacterState::Idle;

};

