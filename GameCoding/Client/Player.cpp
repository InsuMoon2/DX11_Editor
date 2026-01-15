#include "pch.h"
#include "Player.h"
#include "Model.h"
#include "ModelAnimator.h"
#include "Camera.h"
#include "StateComponent.h"

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

    // State Component
    AddComponent(make_shared<StateComponent>());

}

void Player::Start()
{
    GameObject::Start();

    _state = GetScriptComponent<StateComponent>();
    if (!_state) return;
}

void Player::Update()
{
    UpdateInput();
    UpdateAnimation();

    GetTransform()->UpdateTransform();

    GameObject::Update();

    LOG_INFO("playerState : " + string(magic_enum::enum_name(_state->GetState())), 1);
}

void Player::UpdateInput()
{
    auto transform = GetTransform();
    Vec3 pos = transform->GetLocalPosition();
    Vec3 rot = transform->GetLocalRotation();

    if (_state->GetState() == CharacterState::Attack)
        return;

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
        pos += transform->GetRight() * _moveSpeed * DT;
        isMoving = true;
    }
    if (INPUT->GetButton(KEY_TYPE::D))
    {
        pos -= transform->GetRight() * _moveSpeed * DT;
        isMoving = true;
    }

    // 공격
    if (INPUT->GetButtonDown(KEY_TYPE::SPACE))
    {
        _state->ChangeState(CharacterState::Attack);
    }
    else if (isMoving)
    {
        _state->ChangeState(CharacterState::Run);
    }
    else if (_state->GetState() != CharacterState::Attack)
    {
        _state->ChangeState(CharacterState::Idle);
    }

    transform->SetLocalPosition(pos);
    transform->SetLocalRotation(rot);
}

void Player::UpdateAnimation()
{
    if (!_state->IsStateChanged())
        return;

    GetModelAnimator()->SetNextAnimation((int32)_state->GetState());

    _state->ClearStateChanged();
}
