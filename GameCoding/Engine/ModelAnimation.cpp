#include "pch.h"
#include "ModelAnimation.h"

shared_ptr<ModelKeyframe> ModelAnimation::GetKeyframe(const wstring& name)
{
    auto findit = keyframes.find(name);
    if (findit == keyframes.end())
        return nullptr;

    return findit->second;
}
