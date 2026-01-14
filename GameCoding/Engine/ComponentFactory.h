#pragma once

#include "Component.h"

class ComponentFactory
{
public:
    using ComponentLoader = function <void(shared_ptr<GameObject> , const json&)>;

    static void Register(const string& name, ComponentLoader loader);
    static void LoadComponent(shared_ptr<GameObject> obj, const json& json);

    static map<string, ComponentLoader>& GetLoaders();

};

#define REGISTER_COMPONENT(TYPE, ENUM) \
    static struct Helper_Comp_##TYPE { \
        Helper_Comp_##TYPE() { \
            ComponentFactory::GetLoaders()[#TYPE] = [](std::shared_ptr<GameObject> obj, const json& j) { \
                auto comp = obj->GetFixedComponent(ENUM); \
                if (!comp) { \
                    auto newComp = std::make_shared<TYPE>(); \
                    obj->AddComponent(newComp); \
                    comp = newComp; \
                } \
                comp->FromJson(j); \
            }; \
        } \
    } helper_comp_##TYPE;
