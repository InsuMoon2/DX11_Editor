#pragma once

#include "GameObject.h"
#include "magic_enum/magic_enum.hpp"

class BlendSpace1D;
class ModelAnimator;
class StateComponent;

class Player : public GameObject
{
public:
    Player();
    virtual ~Player();

public:
    void Awake() override;
    void Start() override;  
    void Update() override; 

private:
    void UpdateInput();
    void UpdateAnimation();

    void KachujinSetting(shared_ptr<Shader> shader);
    void DeidaraSetting(shared_ptr<Shader> shader);

    void CreateBlendSpace();

private:
    // 이동 관련
    float _moveSpeed = 5.f;
    float _rotSpeed = 3.f;

    shared_ptr<Shader> _shader;

    shared_ptr<StateComponent> _state = {};

    shared_ptr<BlendSpace1D> _locomotionBS;
};
