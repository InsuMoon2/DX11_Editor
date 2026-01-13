#pragma once

#include "Component.h"
#include <set>

class Model;
class IAnimNotify;
class IAnimNotifyState;

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
    //virtual void Update() override;

    void SetModel(shared_ptr<Model> model);
    void SetPass(uint8 pass) { _pass = pass; }

    virtual void Update() override;
    void UpdateTweenData();

    void RenderInstancing(shared_ptr<class InstancingBuffer>& buffer);
    InstanceID GetInstanceID();

public:
    int32 GetCurrentAnimIndex() { return _tweenDesc.curr.animIndex; }
    void SetNextAnimation(int32 animIndex);

    void SetTweenDesc(const TweenDesc& desc) { _tweenDesc = desc; }
    TweenDesc& GetTweenDesc() { return _tweenDesc; }

    bool IsPlaying(int32 animIndex);
    bool IsTransitioning() { return _tweenDesc.next.animIndex >= 0; }

    // 현재 애니메이션이 마지막 프레임에 도달했는지
    bool IsAnimationEnd();

    // ─────────────────────────────────────────────
    // 노티파이 콜백
    // ─────────────────────────────────────────────
    using NotifyCallback = function<void(const wstring& notifyName)>;
    using NotifyStateCallback = function<void(const wstring& notifyName, bool isBegin)>;

    void SetNotifyCallback(NotifyCallback callback) { _notifyCallback = callback; }
    void SetNotifyStateCallback(NotifyStateCallback callback) { _notifyStateCallback = callback; }

    wstring GetCurrentAnimationName();

private:
    void CreateTexture();
    void CreateAnimationTransform(uint32 index);
    void CheckNotifies(int prevFrame, int currFrame);

private:
    vector<AnimTransform> _animTransforms;

    ComPtr<ID3D11Texture2D> _texture;
    ComPtr<ID3D11ShaderResourceView> _srv;

private:
    TweenDesc            _tweenDesc;

private:
    shared_ptr<Shader>   _shader;
    uint8                _pass = 0;
    shared_ptr<Model>    _model;

private:
    // ─────────────────────────────────────────────
    // 노티파이 콜백
    // ─────────────────────────────────────────────
    NotifyCallback      _notifyCallback;
    NotifyStateCallback _notifyStateCallback;
    set<int>            _activeNotifyStates; // 현재 활성화된 NotifyState 인덱스

};

