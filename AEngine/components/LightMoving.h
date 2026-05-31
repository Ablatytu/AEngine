#pragma once
#include "AComponent.h"
#include <memory>

class Transform;

class LightMoving : public AComponent
{
	RegisterComponent(LightMoving)
	public:
		bool rigntPos = false;
		bool CanUp = false;
		bool up = false;
		bool CanMoving = true;
		Transform* transform;
		void Update() override;
		void Start() override;
		void OnInspector() override;
};