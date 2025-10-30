#include "pch.h"
#include "Component.h"

#include "GameObject.h"

Component::Component(ComponentType type)
    : _type(type)
{

}

Component::~Component()
{

}

shared_ptr<GameObject> Component::GetGameObject()
{
    return _gameObject.lock();
}

shared_ptr<Transform> Component::GetTransform()
{
    if (auto gameObject = _gameObject.lock())
        return gameObject->GetTransform();

    return nullptr;
}
