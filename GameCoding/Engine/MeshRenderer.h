#pragma once
#include "Component.h"

class Mesh;
class Shader;
class Material;

class MeshRenderer : public Component
{
	using Super = Component;
public:
	MeshRenderer();
	virtual ~MeshRenderer();

	//virtual void Update() override;

    shared_ptr<Mesh> GetMesh() const { return _mesh; }
    shared_ptr<Material> GetMaterial() const { return _material; }

	void SetMesh(shared_ptr<Mesh> mesh) { _mesh = mesh; }
	void SetMaterial(shared_ptr<Material> material) { _material = material; }
    void SetPass(uint8 pass) { _pass = pass; }

    // Legacy
    void SetTexture(shared_ptr<Texture> texture) { }
    void SetShader(shared_ptr<Shader> shader) {  }

    void RenderInstancing(shared_ptr<InstancingBuffer>& buffer);
    InstanceID GetInstanceID();

    shared_ptr<Component> Clone() const override;

private:
    //shared_ptr<Texture> _texture;
    //shared_ptr<Shader> _shader;

	shared_ptr<Mesh> _mesh;
	shared_ptr<Material> _material;

    uint8 _pass = 0;
};

