#pragma once
#include "AComponent.h"
#include "AComponentRef.h"
#include "Transform.h"
#include "GameObject.h"
#include "BreakoutBall.h"

class UIController;

class BreakoutPlayer : public AComponent
{
	AE_REGISTER_CLASS(BreakoutPlayer);
public:
	void Start() override;
	void Update(float deltaTime) override;
	void OnInspector() override;
	float speed = 5.0f;
	AComponentRef<UIController> uiController;
private:
	AComponentRef<Transform> transform;
	AComponentRef<BreakoutBall> ballPlayer;
};