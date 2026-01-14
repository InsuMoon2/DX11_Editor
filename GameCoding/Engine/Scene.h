#pragma once

class Scene
{
public:
    Scene() = default;
    ~Scene() = default;

public:
    virtual void Init() { }
    virtual void Start();
    virtual void Update();
    virtual void Render();
    virtual void LateUpdate();

    virtual void Add_Scene(shared_ptr<GameObject> object);
    virtual void Remove_Scene(shared_ptr<GameObject> object);

    unordered_set<shared_ptr<GameObject>>& GetObjects() { return _objects; }

private:
    unordered_set<shared_ptr<GameObject>> _objects;

    // Cache Camera
    unordered_set<shared_ptr<GameObject>> _cameras;

    // Cache Light
    unordered_set<shared_ptr<GameObject>> _lights;

};

