#pragma once

#include "Component.h"

class Model;
class Shader;
class Material;

class ModelRenderer : public Component
{
    using Super = Component;

public:
    ModelRenderer(shared_ptr<Shader> shader);
    virtual ~ModelRenderer();

    //virtual void Update() override;

    void SetModel(shared_ptr<Model> model);
    void SetPass(uint8 pass) { _pass = pass; }

    shared_ptr<Shader> GetShader() const { return _shader; }
    shared_ptr<Model> GetModel() const { return _model; }

    void RenderInstancing(shared_ptr<class InstancingBuffer>& buffer);
    InstanceID GetInstanceID();

    shared_ptr<Component> Clone() const override;

    // 직렬화
    json ToJson() const override;
    void FromJson(const json& j) override;

private:
    shared_ptr<Shader> _shader;
    uint8              _pass = 0;
    shared_ptr<Model>  _model;
        

};

