#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "../GameAPI/GameABI.h"
#include "EngineAPIContext.h"

Material::Material(const char* name)
{
	const AE_API* ae_api = ENGINEAPICONTEXT::Get();
	if (!ae_api)
		return;
	ae_api->getMaterial(name, &ABIMaterial);
}
Material::Material(AE_GAME_RESOURCES_INFO::AE_MATERIAL ae_material)
{
	ABIMaterial = ae_material;
}
void Material::SetTexture(const char* nameField, Texture& texture)
{
	const AE_API* ae_api = ENGINEAPICONTEXT::Get();
	if (!ae_api)
		return;
	ae_api->setMaterialTexture(ABIMaterial, nameField, texture.ABITexture);
}

void Material::Set(const char* nameField, std::variant<int, float, bool> number)
{
	const AE_API* ae_api = ENGINEAPICONTEXT::Get();
	if (!ae_api)
		return;

	if (std::holds_alternative<int>(number))
		ae_api->setMaterialValueField(nameField, ABIMaterial, &std::get<int>(number), 0);
	else if (std::holds_alternative<bool>(number))
		ae_api->setMaterialValueField(nameField, ABIMaterial, &std::get<bool>(number), 1);
}