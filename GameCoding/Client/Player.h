#pragma once

#include "GameObject.h"

class ModelAnimator;

enum class AnimState
{
    IDLE = 0,
    RUN = 1,
    ATTACK = 2
};

class Player : public GameObject
{

public:
    Player();
    virtual ~Player();

public:
    void Start() override;  
    void Update() override; 

private:
    void UpdateInput();
    void UpdateAnimation();

private:
    // 이동 관련
    float _moveSpeed = 5.f;
    float _rotSpeed = 3.f;

    shared_ptr<Shader> _shader;

    AnimState _currentState = AnimState::IDLE;
    AnimState _prevState = AnimState::IDLE;

};
