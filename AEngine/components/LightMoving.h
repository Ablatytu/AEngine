#pragma once
#include "AComponent.h"
#include <memory>
#include "../core/AEngineAPI.h"

class Transform;

class AENGINE_API LightMoving : public AComponent
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