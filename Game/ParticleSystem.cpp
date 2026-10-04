#include "ParticleSystem.h"
#include "../GameAPI/GameABI.h"
#include "EngineAPIContext.h"
#include "Material.h"

Material ParticleSystem::GetMaterial()
{
	const AE_API* ae_api = ENGINEAPICONTEXT::Get();
	if (!ae_api)
		return {};
	AE_GAME_RESOURCES_INFO::AE_MATERIAL findedMaterial{};
	if(ae_api->getParticleSystemMaterial(ae_component_handle, &findedMaterial))
	{
		return Material(findedMaterial);
	}
	return {};
}

void ParticleSystem::SetMaterial(Material& material)
{
	const AE_API* ae_api = ENGINEAPICONTEXT::Get();
	if (!ae_api)
		return;
	ae_api->setParticleSystemMaterial(ae_component_handle, material.ABIMaterial);

}


