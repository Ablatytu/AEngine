#pragma once
#include "AComponent.h"
#include "AComponentRef.h"
#include "Transform.h"
#include "GameObject.h"
#include "BreakoutBall.h"
#include "RenderImage.h"
#include "Camera.h"
#include "Material.h"

class UIController;
class SpriteRenderer2D;

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
	AComponentRef<SpriteRenderer2D> spritePlayer;
	Material breakoutEffects;
};