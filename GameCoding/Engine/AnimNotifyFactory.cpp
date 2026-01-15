#include "pch.h"
#include "AnimNotifyFactory.h"

void AnimNotifyFactory::RegisterNotify(const string& typeName, NotifyCreator creator)
{
    GetNotifyCreators()[typeName] = creator;
}

shared_ptr<AnimNotify> AnimNotifyFactory::CreateNotify(const string& typeName)
{
    auto& creators = GetNotifyCreators();

    if (creators.find(typeName) != creators.end())
    {
        return creators[typeName]();
    }

    return nullptr;
}

map<string, AnimNotifyFactory::NotifyCreator>& AnimNotifyFactory::GetNotifyCreators()
{
    static map<string, NotifyCreator> creators;

    return creators;
}

void AnimNotifyFactory::RegisterNotifyState(const string& typeName, NotifyStateCreator creator)
{
    GetNotifyStateCreators()[typeName] = creator;
}

shared_ptr<AnimNotifyState> AnimNotifyFactory::CreateNotifyState(const string& typeName)
{
    auto& creators = GetNotifyStateCreators();

    if (creators.find(typeName) != creators.end())
    {
        return creators[typeName]();
    }

    return nullptr;
}

map<string, AnimNotifyFactory::NotifyStateCreator>& AnimNotifyFactory::GetNotifyStateCreators()
{
    static map<string, NotifyStateCreator> creators;

    return creators;
}
