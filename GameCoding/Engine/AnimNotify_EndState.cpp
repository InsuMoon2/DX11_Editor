#include "pch.h"
#include "AnimNotify_EndState.h"
#include "ModelAnimator.h"
//#include "StateComponent.h"

//REGISTER_ANIM_NOTIFY(AnimNotify_EndState);

wstring AnimNotify_EndState::GetDisplayName() const
{
    return L"End_State";
}

void AnimNotify_EndState::OnNotify(ModelAnimator* animator)
{
    shared_ptr<GameObject> owner = animator->GetOwner();

    auto stateComp = owner->GetScriptComponent<StateComponent>()
}
