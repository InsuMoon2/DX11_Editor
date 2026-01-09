#pragma once

#include "IExecute.h"

class TweenDemo : public IExecute
{
public:
    void Init() override;
    void Update() override;
    void Render() override;

    void CreateKachujin();

    void UpdateInput();      // 입력 처리
    void UpdateCamera();     // 카메라 Follow

private:
    shared_ptr<Shader> _shader;
    shared_ptr<GameObject> _obj;
    shared_ptr<GameObject> _camera;

    float _moveSpeed = 50.f;        // 이동 속도

    // 애니메이션 인덱스 상수
    enum AnimState
    {
        ANIM_IDLE = 0,
        ANIM_RUN = 1,
        ANIM_SLASH = 2
    };

    bool _isAttacking = false;  // 공격 중인지
};

