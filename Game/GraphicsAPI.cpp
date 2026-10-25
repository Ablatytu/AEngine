#pragma once
#include "GraphicsAPI.h"
#include "EngineAPIContext.h"
#include "RenderImage.h"
#include "Camera.h"
#include "Material.h"

void GraphicsAPI::Blit(RenderImage* source, RenderImage* destination, Material* mat, Camera* cam)
{
	const AE_API* ae_api = ENGINEAPICONTEXT::Get();
	if (!ae_api)
		return;
	if (!source || !destination || !cam)
		return;

	if(!mat)
		ae_api->blitRenderFrame(source->ABIRenderImage, destination->ABIRenderImage, cam->GetHandle(), {});
	else
		ae_api->blitRenderFrame(source->ABIRenderImage, destination->ABIRenderImage, cam->GetHandle(),  mat->ABIMaterial);
}