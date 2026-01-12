#pragma once

#include "Event.h"

class GameObject;

class GameObjectCreateEvent : public Event
{
public:
    EVENT_TYPE(GameObject_Created);

    GameObjectCreateEvent(shared_ptr<GameObject> obj);
    shared_ptr<GameObject> GetGameObject() const { return _gameObject; }

private:
    shared_ptr<GameObject> _gameObject;
};

class GameObjectDestroyedEvent : public Event
{
public:
    EVENT_TYPE(GameObject_Destroyed);

    GameObjectDestroyedEvent(shared_ptr<GameObject> obj);
    shared_ptr<GameObject> GetGameObject() const { return _gameObject; }

private:
    shared_ptr<GameObject> _gameObject;
};

