#include "pch.h"
#include "ANS_CollisionEnable.h"
#include "AnimNotifyFactory.h"

REGISTER_ANIM_NOTIFY_STATE(ANS_CollisionEnable, "CollisionEnable");

ANS_CollisionEnable::ANS_CollisionEnable()
{
    Register("testInt", &_testInt);
}

wstring ANS_CollisionEnable::GetDisplayName() const
{
    return L"CollisionEnable";
}

void ANS_CollisionEnable::OnNotifyBegin(ModelAnimator* animator)
{
    LOG_INFO("ANS : Test Log - CollisionEnable Begin");
}

void ANS_CollisionEnable::OnNotifyTick(ModelAnimator* animator, float deltaTime)
{
    // LOG_INFO("ANS : CollisionEnable Tick");
}

void ANS_CollisionEnable::OnNotifyEnd(ModelAnimator* animator)
{
    LOG_INFO("TANS : TestLog - CollisionEnable End");
}
