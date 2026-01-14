#include "pch.h"
#include "FollowCamera.h"

void FollowCamera::LateUpdate()
{
    if (_target == nullptr)
        return;

    Vec3 targetPos = _target->GetTransform()->GetPosition();

    // 목표 위치 계산
    Vec3 desiredPos = targetPos + _offset;

    Vec3 currentPos = GetTransform()->GetPosition();
    GetTransform()->SetPosition(desiredPos);
    //Vec3 smoothedPos = Vec3::Lerp(currentPos, desiredPos, DT * _smoothSpeed);
    //GetTransform()->SetPosition(smoothedPos);

    GetTransform()->LookAt(targetPos);
}
