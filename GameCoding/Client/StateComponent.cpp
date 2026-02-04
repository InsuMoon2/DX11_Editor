#include "pch.h"
#include "StateComponent.h"

StateComponent::StateComponent()
{
}

StateComponent::~StateComponent()
{
}

void StateComponent::Update()
{
    MonoBehavior::Update();


}

void StateComponent::ChangeState(CharacterState newState)
{
    if (_state == newState)
        return; 

    _prevState = _state;

    _state = newState;
    _stateChanged = true;
}
