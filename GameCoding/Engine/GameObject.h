#pragma once
#include "Component.h"

class FollowCamera;
class MonoBehavior;
class Transform;
class Camera;
class MeshRenderer;
class ModelRenderer;
class ModelAnimator;
class Light;

class GameObject : public enable_shared_from_this<GameObject>
{
public:
	GameObject();
	~GameObject();

	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FixedUpdate();

	shared_ptr<Component>       GetFixedComponent(ComponentType type);
	shared_ptr<Transform>       GetTransform();
	shared_ptr<Camera>          GetCamera();
	shared_ptr<MeshRenderer>    GetMeshRenderer();
    shared_ptr<ModelRenderer>   GetModelRenderer();
	shared_ptr<ModelAnimator>   GetModelAnimator();
	shared_ptr<Light>           GetLight();

	shared_ptr<Transform> GetOrAddTransform();
	void AddComponent(shared_ptr<Component> component);

    // 컴포넌트 배열 (복제용)
    const array<shared_ptr<Component>, FIXED_COMPONENT_COUNT>& GetComponents() const
    {
        return _components;
    }

    template<typename T>
    shared_ptr<T> GetScriptComponent()
    {
        for (auto& script : _scripts)
        {
            shared_ptr<T> component = dynamic_pointer_cast<T>(script);

            if (component)
                return component;
        }

        return nullptr;
    }

    // 게임 오브젝트 복제
    shared_ptr<GameObject> Clone() const;

    // Name
    void SetName(const wstring& name) { _name = name; }
    wstring GetName() { return _name; }

protected:
	array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> _components;
	vector<shared_ptr<MonoBehavior>> _scripts;

    wstring _name = L"GameObject";
};

