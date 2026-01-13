#include "pch.h"
#include "Player.h"
#include "Model.h"
#include "ModelAnimator.h"
#include "Camera.h"

Player::Player()
{ }

Player::~Player()
{ }

void Player::Init(shared_ptr<Shader> shader)
{
    // 모델 로드
    auto model = make_shared<Model>();
    model->ReadModel(L"Kachujin/Kachujin");
    model->ReadMaterial(L"Kachujin/Kachujin");
    model->ReadAnimation(L"Kachujin/Idle");
    model->ReadAnimation(L"Kachujin/Run");
    model->ReadAnimation(L"Kachujin/Slash");

    // Transform 설정
    GetOrAddTransform()->SetScale(Vec3(0.01f));

    // ModelAnimator 추가
    AddComponent(make_shared<ModelAnimator>(shader));
    GetModelAnimator()->SetModel(model);
    GetModelAnimator()->SetPass(2);

    // 카메라 추가
    _camera = make_shared<GameObject>();
    _camera->GetOrAddTransform()->SetPosition(Vec3(0, 5, -10));
    _camera->AddComponent(make_shared<Camera>());

}

void Player::Update()
{
    UpdateInput();
    UpdateAnimation();

    GetTransform()->UpdateTransform();

    GameObject::Update();

    Vec3 playerPos = GetTransform()->GetPosition();

    _camera->GetTransform()->SetPosition(playerPos + _cameraOffset);
    _camera->GetTransform()->LookAt(playerPos);
    _camera->GetTransform()->UpdateTransform();
    _camera->Update();

}

void Player::Render()
{

}

void Player::UpdateInput()
{
    auto transform = GetTransform();
    Vec3 pos = transform->GetPosition();
    Vec3 rot = transform->GetRotation();

    bool isMoving = false;

    // WASD 이동
    if (INPUT->GetButton(KEY_TYPE::W))
    {
        pos += transform->GetLook() * _moveSpeed * DT;
        isMoving = true;
    }
    if (INPUT->GetButton(KEY_TYPE::S))
    {
        pos -= transform->GetLook() * _moveSpeed * DT;
        isMoving = true;
    }
    if (INPUT->GetButton(KEY_TYPE::A))
    {
        rot.y -= _rotSpeed * DT;
    }
    if (INPUT->GetButton(KEY_TYPE::D))
    {
        rot.y += _rotSpeed * DT;
    }

    // 공격
    if (INPUT->GetButtonDown(KEY_TYPE::SPACE))
    {
        _currentState = AnimState::ATTACK;
    }
    else if (isMoving) {
        _currentState = AnimState::RUN;
    }
    else if (_currentState != AnimState::ATTACK)
    {
        _currentState = AnimState::IDLE;
    }

    // 공격 애니메이션 종료 체크
    if (_currentState == AnimState::ATTACK)
    {
        if (GetModelAnimator()->IsAnimationEnd())
            _currentState = AnimState::IDLE;
    }

    transform->SetPosition(pos);
    transform->SetRotation(rot);
}

void Player::UpdateAnimation() {
    if (_currentState == _prevState)
        return;

    GetModelAnimator()->SetNextAnimation((int32)_currentState);
    _prevState = _currentState;
}
