#include "pch.h"
#include "AnimNotifyFactory.h"

void AnimNotifyFactory::Register(const string& typeName, NotifyCreator creator)
{
    GetCreators()[typeName] = creator;
}

shared_ptr<AnimNotify> AnimNotifyFactory::CreateNotify(const string& typeName)
{
    auto& creators = GetCreators();

    if (creators.find(typeName) != creators.end())
    {
        return creators[typeName]();
    }

    return nullptr;
}

map<string, AnimNotifyFactory::NotifyCreator>& AnimNotifyFactory::GetCreators()
{
    static map<string, NotifyCreator> creators;

    return creators;
}
