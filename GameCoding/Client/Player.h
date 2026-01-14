#pragma once

#include "GameObject.h"
#include "magic_enum/magic_enum.hpp"

class ModelAnimator;

enum class AnimState : uint8
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
    void Awake() override;
    void Start() override;  
    void Update() override; 

private:
    void UpdateInput();
    void UpdateAnimation();

    void OnAnimNotify(const wstring& name);

private:
    // 이동 관련
    float _moveSpeed = 5.f;
    float _rotSpeed = 3.f;

    shared_ptr<Shader> _shader;

    AnimState _currentState = AnimState::IDLE;
    AnimState _prevState = AnimState::IDLE;

};
