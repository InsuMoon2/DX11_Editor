#include "pch.h"
#include "Player.h"
#include "Model.h"
#include "ModelAnimator.h"
#include "Camera.h"

REGISTER_GAMEOBJECT(Player)

Player::Player()
{
    SetName(L"Player");
}

Player::~Player()
{ }

void Player::Awake()
{
    GameObject::Awake();

    auto shader = GET_SINGLE(RenderManager)->GetShader();

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

    vector<wstring> animPaths =
    {
    L"Kachujin/Idle",
    L"Kachujin/Run",
    L"Kachujin/Slash"
    };

    GET_SINGLE(ModelRegistry)->RegisterModel(
        L"Kachujin",
        model,
        GetModelAnimator(),
        animPaths
    );
}

void Player::Start()
{
    GameObject::Start();

    
}

void Player::Update()
{
    UpdateInput();
    UpdateAnimation();

    GetTransform()->UpdateTransform();

    GameObject::Update();

    LOG_INFO("playerState : " + string(magic_enum::enum_name(_currentState)), 1);
}

void Player::UpdateInput()
{
    auto transform = GetTransform();
    Vec3 pos = transform->GetLocalPosition();
    Vec3 rot = transform->GetLocalRotation();

    bool isMoving = false;

    // WASD 이동
    if (INPUT->GetButton(KEY_TYPE::W))
    {
        pos += transform->GetForward() * _moveSpeed * DT;
        isMoving = true;
    }
    if (INPUT->GetButton(KEY_TYPE::S))
    {
        pos -= transform->GetForward() * _moveSpeed * DT;
        isMoving = true;
    }
    if (INPUT->GetButton(KEY_TYPE::A))
    {
        pos += transform->GetRight() * DT;
    }
    if (INPUT->GetButton(KEY_TYPE::D))
    {
        pos -= transform->GetRight() * DT;
    }

    // 공격
    if (INPUT->GetButtonDown(KEY_TYPE::SPACE))
    {
        _currentState = AnimState::ATTACK;
    }
    else if (isMoving)
    {
        _currentState = AnimState::RUN;
    }
    else if (_currentState != AnimState::ATTACK)
    {
        _currentState = AnimState::IDLE;
    }

    // 공격 애니메이션 종료 체크 -> 이 부분을 노티파이로 변경시켜줘야함
    // End_State 하고 디테일 창 추가해서 파싱으로 음.. 해결할 수 있으려나
    if (_currentState == AnimState::ATTACK)
    {
        if (GetModelAnimator()->IsAnimationEnd())
            _currentState = AnimState::IDLE;
    }

    transform->SetLocalPosition(pos);
    transform->SetLocalRotation(rot);
}

void Player::UpdateAnimation()
{
    if (_currentState == _prevState)
        return;

    GetModelAnimator()->SetNextAnimation((int32)_currentState);

    _prevState = _currentState;
}

void Player::OnAnimNotify(const wstring& name)
{

}
