#pragma once

#include "Component.h"

class Model;

struct AnimTransform
{
    // [ ][ ][ ][ ] ... 최대 250 개
    using TransformArrayType = array<Matrix, MAX_MODEL_TRANSFORMS>;

    // [ ][ ][ ][ ][ ] ... 500개
    array<TransformArrayType, MAX_MODEL_KEYFRAMES> transforms;
};

class ModelAnimator : public Component
{
    using Super = Component;

public:
    ModelAnimator(shared_ptr<Shader> shader);
    ~ModelAnimator();

public:
    virtual void Update() override;

    void SetModel(shared_ptr<Model> model);
    void SetPass(uint8 pass) { _pass = pass; }

public:
    int32 GetCurrentAnimIndex() { return _tweenDesc.curr.animIndex; }
    void SetNextAnimation(int32 animIndex);

    bool IsPlaying(int32 animIndex);
    bool IsTransitioning() { return _tweenDesc.next.animIndex >= 0; }

    // 현재 애니메이션이 마지막 프레임에 도달했는지
    bool IsAnimationEnd();

private:
    void CreateTexture();
    void CreateAnimationTransform(uint32 index);

private:
    vector<AnimTransform> _animTransforms;

    ComPtr<ID3D11Texture2D> _texture;
    ComPtr<ID3D11ShaderResourceView> _srv;

private:
    KeyframeDesc _keyframeDesc;
    TweenDesc    _tweenDesc;

private:
    shared_ptr<Shader> _shader;
    uint8              _pass = 0;
    shared_ptr<Model>  _model;

};

