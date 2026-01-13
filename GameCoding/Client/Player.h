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
    void Init(shared_ptr<Shader> shader);
    void Update();
    void Render();

private:
    void UpdateInput();
    void UpdateAnimation();

private:
    // 이동 관련
    float _moveSpeed = 5.f;
    float _rotSpeed = 3.f;

    AnimState _currentState = AnimState::IDLE;
    AnimState _prevState = AnimState::IDLE;

    shared_ptr<GameObject> _camera;
    Vec3 _cameraOffset = Vec3(0, 5, -10);
};
