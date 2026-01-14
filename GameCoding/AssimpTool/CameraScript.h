#pragma once
#include "MonoBehavior.h"

class CameraScript : public MonoBehavior
{
public:
	virtual void Start() override;
	virtual void Update() override;

	float _speed = 10.f;
};

