#pragma once

#include "Scene.h"

class SceneManager
{
    DECLARE_SINGLE(SceneManager)

public:
    void Update();

    shared_ptr<Scene> GetCurrentScene() { return _currentScene; }

public:
    template<typename T>
    void ChangeScene(shared_ptr<T> scene)
    {
        _currentScene = scene;
        scene->Start();
    }


private:
    shared_ptr<Scene> _currentScene = make_shared<Scene>();

};

