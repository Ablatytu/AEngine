#include "AComponent.h"
#include <string>

class PowerUp : public AComponent
{
	AE_REGISTER_CLASS(PowerUp);
public:
	std::string Type = "";
	float Duraction = 0.0f;
	bool Activated = false;

	PowerUp() = default;
	PowerUp(std::string type, float duraction) : Type(type), Duraction(duraction) {}
};