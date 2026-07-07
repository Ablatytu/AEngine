#pragma once
#include "AComponent.h"
#include "../core/AEngineAPI.h"

class AENGINE_API Rei : public AComponent
{
	RegisterComponent(Rei)
public:
	float speedRange = 0.0f;
	float speedWithoutRange = 0.0f;
	bool flag = false;
	int countRange = 0;
	int countWithoutRange = 0;
	void OnInspector() override;
};