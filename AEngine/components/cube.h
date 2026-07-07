#pragma once
#include "Transform.h"
#include <memory>
#include "../core/AEngineAPI.h"

class AENGINE_API Cube : public AComponent
{
	RegisterComponent(Cube)
public:
	Transform* transform;
	bool rotate = false;
	void Update() override;
	void Start() override;
	void OnInspector() override;
};