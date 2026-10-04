#include "SpriteRenderer2D.h"
#include "Texture.h"
#include "EngineAPIContext.h"

void SpriteRenderer2D::setTexture(Texture texture)
{
	const AE_API* ae_api = ENGINEAPICONTEXT::Get();
	if (!ae_api)
		return;

	ae_api->setSpriteTexture(texture.ABITexture.name, ae_component_handle);
}

void SpriteRenderer2D::setColor(Vector4 color)
{
	const AE_API* ae_api = ENGINEAPICONTEXT::Get();
	if (!ae_api)
		return;

	ae_api->setSpriteColor(AE_VEC4{ color.x, color.y, color.z, color.w }, ae_component_handle);
}