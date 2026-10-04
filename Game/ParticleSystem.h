#pragma once
#include "AComponent.h"
#include "Material.h"

class ParticleSystem final : public AComponent
{
public:
	ParticleSystem() = default;

	Material GetMaterial();
	void SetMaterial(Material& material);
};