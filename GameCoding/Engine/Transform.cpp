#include "pch.h"
#include "Transform.h"

REGISTER_COMPONENT(Transform, ComponentType::Transform);

Transform::Transform() : Super(ComponentType::Transform)
{

}

Transform::~Transform()
{

}

void Transform::Awake()
{
}

void Transform::Update()
{
}

Vec3 ToEulerAngles(Quaternion q)
{
    Vec3 angles;
    // X -> Y -> Z 순서 (Rx * Ry * Rz)

    // x (Pitch)
    double sinx = -2 * (q.y * q.z - q.w * q.x);
    if (std::abs(sinx) >= 1)
        angles.x = std::copysign(3.141592f / 2, sinx);
    else
        angles.x = std::asin(sinx);
    // y (Yaw)
    double siny_cosx = 2 * (q.x * q.z + q.w * q.y);
    double cosy_cosx = 1 - 2 * (q.x * q.x + q.y * q.y);
    angles.y = std::atan2(siny_cosx, cosy_cosx);
    // z (Roll)
    double sinz_cosx = 2 * (q.x * q.y + q.w * q.z);
    double cosz_cosx = 1 - 2 * (q.x * q.x + q.z * q.z);
    angles.z = std::atan2(sinz_cosx, cosz_cosx);
    return angles;
}

void Transform::UpdateTransform()
{
	Matrix matScale = Matrix::CreateScale(_localScale);
	Matrix matRotation = Matrix::CreateRotationX(_localRotation.x);
	matRotation *= Matrix::CreateRotationY(_localRotation.y);
	matRotation *= Matrix::CreateRotationZ(_localRotation.z);
	Matrix matTranslation = Matrix::CreateTranslation(_localPosition);

	_matLocal = matScale * matRotation * matTranslation;

	if (HasParent())
	{
		_matWorld = _matLocal * _parent->GetWorldMatrix();
	}
	else
	{
		_matWorld = _matLocal;
	}

	Quaternion quat;
	_matWorld.Decompose(_scale, quat, _position);
	_rotation = ToEulerAngles(quat);

	// Children
	for (const shared_ptr<Transform>& child : _children)
		child->UpdateTransform();
}

void Transform::SetScale(const Vec3& worldScale)
{
	if (HasParent())
	{
		Vec3 parentScale = _parent->GetScale();
		Vec3 scale = worldScale;
		scale.x /= parentScale.x;
		scale.y /= parentScale.y;
		scale.z /= parentScale.z;
		SetLocalScale(scale);
	}
	else
	{
		SetLocalScale(worldScale);
	}
}

void Transform::SetRotation(const Vec3& worldRotation)
{
	if (HasParent())
	{
		Matrix inverseMatrix = _parent->GetWorldMatrix().Invert();

		Vec3 rotation;
		rotation.TransformNormal(worldRotation, inverseMatrix);

		SetLocalRotation(rotation);
	}
	else
		SetLocalRotation(worldRotation);
}

void Transform::SetPosition(const Vec3& worldPosition)
{
	if (HasParent())
	{
		Matrix worldToParentLocalMatrix = _parent->GetWorldMatrix().Invert();

		Vec3 position;
		position.Transform(worldPosition, worldToParentLocalMatrix);

		SetLocalPosition(position);
	}
	else
	{
		SetLocalPosition(worldPosition);
	}
}

void Transform::LookAt(const Vec3& target)
{
    Vec3 direction = target - _position;

    // 방향이 너무 짧으면 무시
    if (direction.LengthSquared() < 0.0001f)
        return;

    direction.Normalize();

    // Y축 회전 (Yaw)
    float yaw = atan2(direction.x, direction.z);

    // X축 회전 (Pitch)
    float pitch = -asin(direction.y);

    SetRotation(Vec3(pitch, yaw, 0.f));
}

void Transform::Translate(const Vec3& translation)
{
    SetPosition(_position + translation);
}

void Transform::TranslateLocal(const Vec3& translation)
{
    // 로컬 방향 기준으로 이동
    Vec3 worldTranslation =
        GetRight() * translation.x +
        GetUp() * translation.y +
        GetLook() * translation.z;

    SetPosition(_position + worldTranslation);
}

void Transform::Rotate(const Vec3& euler)
{
    SetRotation(_rotation + euler);
}

shared_ptr<Component> Transform::Clone() const
{
    auto clone = make_shared<Transform>();

    clone->SetLocalPosition(_localPosition + Vec3(1, 0, 0)); // 살짝 옆으로 복사
    clone->SetLocalRotation(_localRotation);
    clone->SetLocalScale(_localScale);

    return clone;
}

json Transform::ToJson() const
{
    json j;

    j =
    {
        {"type", "Transform"},
        {"position", {_localPosition.x, _localPosition.y, _localPosition.z}},
        {"rotation", {_localRotation.x, _localRotation.y, _localRotation.z}},
        {"scale", {_localScale.x, _localScale.y, _localScale.z}}
    };

    return j;
}

void Transform::FromJson(const json& j)
{
    if (j.contains("position"))
    {
        auto& pos = j["position"];
        SetLocalPosition(Vec3(pos[0], pos[1], pos[2]));
    }
    if (j.contains("rotation"))
    {
        auto& rot = j["rotation"];
        SetLocalRotation(Vec3(rot[0], rot[1], rot[2]));
    }
    if (j.contains("scale"))
    {
        auto& scale = j["scale"];
        SetLocalScale(Vec3(scale[0], scale[1], scale[2]));
    }
}
