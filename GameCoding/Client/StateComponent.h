#pragma once

#include "MonoBehavior.h"

enum class CharacterState
{
    Idle,
    Run,
    Attack,

    End
};

class StateComponent : public MonoBehavior
{
public:
    StateComponent();
    ~StateComponent();

public:
    virtual void Update() override;

    void SetState(CharacterState state) { _state = state; }
    CharacterState GetState() { return _state; }

private:
    CharacterState _state = CharacterState::Idle;

};
