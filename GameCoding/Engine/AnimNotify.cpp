#include "pch.h"
#include "AnimNotify.h"

AnimNotify::AnimNotify()
{
    Register("frame", &_frame);
}

void AnimNotify::OnGui()
{
    ImGui::Separator();
}

void AnimNotify::Serialize(json& j) const
{
    for (auto& prop : _properties)
    {
        prop.serialize(j);
    }
}

void AnimNotify::Deserialize(const json& j)
{
    for (auto& prop : _properties)
        prop.deserialize(j);
}
