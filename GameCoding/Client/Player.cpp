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

    auto model = make_shared<Model>();

    // Kachujin
    {
        //model->ReadModel(L"Kachujin/Kachujin");
        //model->ReadMaterial(L"Kachujin/Kachujin");
        //model->ReadAnimation(L"Kachujin/Idle");
        //model->ReadAnimation(L"Kachujin/Run");
        //model->ReadAnimation(L"Kachujin/Slash");
    }

    // Deidara
    {
        model->ReadModel(L"Deidara/Deidara");
        model->ReadMaterial(L"Deidara/Deidara");
        model->ReadAnimation(L"Deidara/aerial");
        model->ReadAnimation(L"Deidara/aerial2");
        model->ReadAnimation(L"Deidara/atk");
        model->ReadAnimation(L"Deidara/dragon");
        model->ReadAnimation(L"Deidara/dragon2");

    }

    // Transform 설정
    GetOrAddTransform()->SetScale(Vec3(0.01f));

    // ModelAnimator 추가
    AddComponent(make_shared<ModelAnimator>(shader));
    GetModelAnimator()->SetModel(model);
    GetModelAnimator()->SetPass(2);

    //vector<wstring> animPaths =
    //{
    //L"Kachujin/Idle",
    //L"Kachujin/Run",
    //L"Kachujin/Slash"
    //};

    //GET_SINGLE(ModelRegistry)->RegisterModel(
    //    L"Kachujin",
    //    model,
    //    GetModelAnimator(),
    //    animPaths
    //);

    vector<wstring> animPaths = {
    L"Deidara/aerial",
    L"Deidara/aerial2",
    L"Deidara/atk",
    L"Deidara/dragon",
    L"Deidara/dragon2"
    };

    GET_SINGLE(ModelRegistry)->RegisterModel(
        L"Deidara",
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

    //GetTransform()->UpdateTransform();

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

    // 입력 방향 계산
    Vec3 inputDir = Vec3::Zero;
    if (INPUT->GetButton(KEY_TYPE::W)) inputDir.z += 1.f;
    if (INPUT->GetButton(KEY_TYPE::S)) inputDir.z -= 1.f;
    if (INPUT->GetButton(KEY_TYPE::A)) inputDir.x -= 1.f;
    if (INPUT->GetButton(KEY_TYPE::D)) inputDir.x += 1.f;

    // 이동 입력이 있으면
    if (inputDir.LengthSquared() > 0.f)
    {
        inputDir.Normalize();

        // 목표 회전각 계산
        float targetYaw = atan2f(inputDir.x, inputDir.z);

        // 최단 경로 계산
        float angleDiff = targetYaw - rot.y;

        while (angleDiff > XM_PI)
            angleDiff -= XM_2PI;

        while (angleDiff < -XM_PI)
            angleDiff += XM_2PI;

        // Lerp
        rot.y = rot.y + angleDiff * _rotSpeed * DT;
        transform->SetLocalRotation(rot);

        pos += transform->GetForward() * _moveSpeed * DT;
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
}

void Player::UpdateAnimation()
{
    if (!_state->IsStateChanged())
        return;

    GetModelAnimator()->SetNextAnimation((int32)_state->GetState());

    _state->ClearStateChanged();
}

