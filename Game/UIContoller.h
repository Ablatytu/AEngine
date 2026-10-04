#pragma once
#include "AComponent.h"
#include <vector>

class BreakoutLevel;

class UIController : public AComponent
{
	AE_REGISTER_CLASS(UIController);
public:
	void Start() override;
	std::vector<AComponentRef<BreakoutLevel>> Levels;
};