#include "pch.h"
#include "GameObjectEvent.h"

GameObjectCreateEvent::GameObjectCreateEvent(shared_ptr<GameObject> obj)
    : _gameObject(obj)
{ }

GameObjectDestroyedEvent::GameObjectDestroyedEvent(shared_ptr<GameObject> obj)
    : _gameObject(obj)
{ }
