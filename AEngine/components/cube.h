#pragma once
#include "Transform.h"
#include <memory>

class Cube : public AComponent
{
	RegisterComponent(Cube)
public:
	Transform* transform;
	bool rotate = false;
	void Update() override;
	void Start() override;
	void OnInspector() override;
};