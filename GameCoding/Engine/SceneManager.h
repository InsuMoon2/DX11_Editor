#pragma once

#include "Scene.h"

class SceneManager
{
    DECLARE_SINGLE(SceneManager)

public:
    void Update();
    void Render();

    shared_ptr<Scene> GetCurrentScene() { return _currentScene; }

public:
    void Play()         { _isPlaying = true; _isPaused = false; }
    void Pause()        { _isPaused = !_isPaused; }
    void Stop()         { _isPlaying = false; _isPaused = false; }
    bool IsPlaying()    { return _isPlaying; }
    bool IsPaused()     { return _isPaused; }

public:
    template <typename T> void ChangeScene(shared_ptr<T> scene)
    {
        _currentScene = scene;
        scene->Init();
        scene->Start();
    }

private:
    shared_ptr<Scene> _currentScene = make_shared<Scene>();

    bool _isPlaying = true;
    bool _isPaused = false;
};
