#include "pch.h"
#include "GameObjectFactory.h"

map<string, GameObjectFactory::GameObjectCreator>& GameObjectFactory::GetCreators()
{
    static map<string, GameObjectCreator> creators;

    return creators;
}

void GameObjectFactory::RegisterCreateType(const string& name, GameObjectCreator creator)
{
    GetCreators()[name] = creator;
}

shared_ptr<GameObject> GameObjectFactory::CreateObject(const string& name)
{
    auto& creators = GetCreators();

    if (creators.find(name) == creators.end())
        return nullptr;

    shared_ptr<GameObject> object = creators[name]();
    if (object)
    {
        object->Awake();
    }

    return object;
}

const map<string, GameObjectFactory::GameObjectCreator>& GameObjectFactory::GetAllCreator()
{
    return GetCreators();
}


