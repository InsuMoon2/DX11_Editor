#include "pch.h"
#include "ComponentFactory.h"

void ComponentFactory::Register(const string& name, ComponentLoader loader)
{
    GetLoaders()[name] = loader;
}

void ComponentFactory::LoadComponent(shared_ptr<GameObject> obj, const json& json)
{
    string type = json["type"];

    // 등록된 로더가 있으면 실행
    if (GetLoaders().find(type) != GetLoaders().end())
    {
        GetLoaders()[type](obj, json);
    }
}

map<string, ComponentFactory::ComponentLoader>& ComponentFactory::GetLoaders()
{
    static map<string, ComponentLoader> loaders;

    return loaders;
}
