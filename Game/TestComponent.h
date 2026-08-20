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
	float speed = 0.0f;
	bool CPLUSPLUS = true;
	int testInt = 5;
	Vector3 vec3Test = {2.0f, 0.0f, 5.0f};
	AComponentRef<Transform> transform;
};