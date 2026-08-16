#pragma once
#include "AComponent.h"
#include "AComponentRef.h"
#include "Transform.h"

class TestComponent : public AComponent
{
	AE_REGISTER_CLASS(TestComponent);
public:
	void OnInspector() override;
	void Update(float deltaTime) override;
	void Start() override;
private:
	bool move = true;
	bool DLLCPLUSPLUSSCRIPTING = true;
	float speed = 0.0f;
	//int testInt = 0;
	Vector3 testVec3 = { 0.0f, 0.0f, 0.0f };
	AComponentRef<Transform> transform;
};