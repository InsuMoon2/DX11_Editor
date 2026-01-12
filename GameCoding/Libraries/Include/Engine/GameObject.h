#pragma once
#include "Component.h"

class MonoBehaviour;
class Transform;
class Camera;
class MeshRenderer;
class ModelRenderer;
class ModelAnimator;

class GameObject : public enable_shared_from_this<GameObject>
{
public:
	GameObject();
	~GameObject();

	void Awake();
	void Start();
	void Update();
	void LateUpdate();
	void FixedUpdate();

	shared_ptr<Component>       GetFixedComponent(ComponentType type);
	shared_ptr<Transform>       GetTransform();
	shared_ptr<Camera>          GetCamera();
	shared_ptr<MeshRenderer>    GetMeshRenderer();
    shared_ptr<ModelRenderer>   GetModelRenderer();
	shared_ptr<ModelAnimator>        GetModelAnimator();

	shared_ptr<Transform> GetOrAddTransform();
	void AddComponent(shared_ptr<Component> component);

    // 컴포넌트 배열 (복제용)
    const array<shared_ptr<Component>, FIXED_COMPONENT_COUNT>& GetComponents() const
    {
        return _components;
    }

    // 게임 오브젝트 복제
    shared_ptr<GameObject> Clone() const;

protected:
	array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> _components;
	vector<shared_ptr<MonoBehaviour>> _scripts;
};

