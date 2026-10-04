#pragma once
#include "AComponent.h"
#include "AComponentRef.h"
#include "Transform.h"

class Block : public AComponent
{
	AE_REGISTER_CLASS(Block);
public:
	bool isSolid = false;
	bool isDestroy = false;
	void Start() override;
	AComponentRef<Transform> transform;
};