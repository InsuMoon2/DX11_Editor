#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Camera.h"
#include "Light.h"

void Scene::Start()
{
    auto objects = _objects;

    for (shared_ptr<GameObject> object : objects)
    {
        object->Start();
    }

}

void Scene::Update()
{
    auto objects = _objects;

    for (shared_ptr<GameObject> object : objects)
    {
        object->Update();
    }

}

void Scene::Render()
{
    // Instancing
    vector<shared_ptr<GameObject>> temp;
    temp.insert(temp.end(), _objects.begin(), _objects.end());
    INSTANCING->Render(temp);
}

void Scene::LateUpdate()
{
    auto objects = _objects;

    for (shared_ptr<GameObject> object : objects)
    {
        object->LateUpdate();
    }
}

void Scene::Add_Scene(shared_ptr<GameObject> object)
{
    _objects.insert(object);

    if (object->GetCamera() != nullptr)
    {
        _cameras.insert(object);
    }

    if (object->GetLight() != nullptr)
    {
        _lights.insert(object);
    }
}

void Scene::Remove_Scene(shared_ptr<GameObject> object)
{
    _objects.erase(object);

    _cameras.erase(object);

    _lights.erase(object);

}

void Scene::Clear()
{
    _objects.clear();

    _cameras.clear();

    _lights.clear();
}
