#pragma once

#include "GameObject.h"

class GameObjectFactory
{
public:
    using GameObjectCreator = function<shared_ptr<GameObject>()>;

    static map<string, GameObjectCreator>& GetCreators();

    // 생성 함수
    static void RegisterCreateType(const string& name, GameObjectCreator creator);
    static shared_ptr<GameObject> CreateObject(const string& name);

    static const map<string, GameObjectCreator>& GetAllCreator();

};

#define REGISTER_GAMEOBJECT(TYPE) \
    static struct Helper_##TYPE { \
        Helper_##TYPE() { \
            GameObjectFactory::RegisterCreateType(#TYPE, []() { return make_shared<TYPE>(); }); \
        } \
    } helper_##TYPE;

