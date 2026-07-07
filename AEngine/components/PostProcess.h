#pragma once
#include "AComponent.h"
#include "../core/AEngineAPI.h"

class Camera;

class AENGINE_API PostProcess : public AComponent
{
	RegisterComponent(PostProcess)
public:
	bool enabled = true;
	float exposure = 1.0f;
	bool AutoExposure = false;
	bool SSAO = true;
	float maxExposure = 15.0f;
	float minExposure = 0.1f;
	float adaptationSpeed = 1.5f;
	float targetBrightness = 0.5f;
	bool Bloom = true;
	float exposureBloom = 1.0f;
	Camera* camera = nullptr;
	void OnInspector() override;
	void Awake() override;
	void OnDestroy() override;
};