#pragma once

class Scene
{
public:
    virtual void Start();
    virtual void Update();
    virtual void LateUpdate();

    virtual void Add(shared_ptr<GameObject> object);
    virtual void Remove(shared_ptr<GameObject> object);

    unordered_set<shared_ptr<GameObject>>& GetObjects() { return _objects; }

private:
    unordered_set<shared_ptr<GameObject>> _objects;

    // Cache Camera
    unordered_set<shared_ptr<GameObject>> _cameras;

    // Cache Light
    unordered_set<shared_ptr<GameObject>> _lights;

};

