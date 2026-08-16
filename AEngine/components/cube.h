#pragma once
#include "Transform.h"
#include <memory>
#include "../core/AEngineAPI.h"
#include "AEngineComponent.h"

class AENGINE_API Cube : public AEngineComponent
{
	AE_REGISTER_CLASS(Cube);
public:
	Transform* transform;
	bool rotate = false;
	void Update() override;
	void Start() override;
	void OnInspector() override;
};