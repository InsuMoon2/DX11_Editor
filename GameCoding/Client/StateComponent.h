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

    void ChangeState(CharacterState newState);
    CharacterState GetState() { return _state; }

    void ClearStateChanged() { _stateChanged = false; }
    bool IsStateChanged() { return _stateChanged; }

private:
    CharacterState _state = CharacterState::Idle;
    CharacterState _prevState = CharacterState::Idle;

    bool _stateChanged = false;

};
