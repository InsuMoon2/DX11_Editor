#include "pch.h"
#include "AnimNotifyState.h"

AnimNotifyState::AnimNotifyState()
{
    Register("startFrame", &_startFrame);
    Register("endFrame", &_endFrame);
}

void AnimNotifyState::Serialize(json& j) const
{
    for (auto& prop : _properties)
        prop.serialize(j);
}

void AnimNotifyState::Deserialize(const json& j)
{
    for (auto& prop : _properties)
        prop.deserialize(j);
}
