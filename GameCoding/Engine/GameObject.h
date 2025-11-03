#pragma once
#include "Component.h"

class Component;
class MonoBehavior;
class Transform;
class Camera;
class MeshRenderer;
class Animator;

class GameObject : public enable_shared_from_this<GameObject>
{
public:
    GameObject();
    virtual ~GameObject();

    void Awake();
    void Start();
    void Update();
    void LateUpdate();
    void FixedUpdate();

    shared_ptr<Component> GetFixedComponent(ComponentType type);
    shared_ptr<Transform> GetTransform();
    shared_ptr<Camera>    GetCamera();
    shared_ptr<MeshRenderer> GetMeshRenderer();
    //shared_ptr<Animator>  GetAnimator();

    shared_ptr<Transform> GetOrAddTransform();
    void AddComponent(shared_ptr<Component> component);

    // void Render(shared_ptr<Pipeline> pipeline);

private:


protected:
    array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> _components;
    vector<shared_ptr<MonoBehavior>> _scripts;

};

