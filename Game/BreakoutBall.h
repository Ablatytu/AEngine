#pragma once
#include "AComponent.h"
#include "Transform.h"
#include "AComponentRef.h"
#include "Vector2.h"

class BreakoutBall : public AComponent
{
	AE_REGISTER_CLASS(BreakoutBall);
public:
	float Radius = 0.125f;
	bool Stuck = true;
	Vector2 Velocity = Vector2(1.0f, 3.5f);
	void Start() override;
	Vector2 Move(float dt, unsigned int width, unsigned int height);
	bool CheckReset();
	void Reset(Vector2 posPlatform, Vector2 scalePlatform);
	AComponentRef<Transform> transform;
};