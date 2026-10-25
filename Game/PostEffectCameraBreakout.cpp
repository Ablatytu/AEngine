#include "PostEffectCameraBreakout.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include "GraphicsAPI.h"

void PostEffectCameraBreakout::Start()
{
	cam = GetGameObject().GetAComponent<Camera>();
	matCam = ResourceManager::Load<Material>("BreakoutEffectsMaterial");
}

bool PostEffectCameraBreakout::OnRenderImage(RenderImage* source, RenderImage* destination, Camera* cam)
{
	matCam.Set("chaos", Chaos);
	matCam.Set("confuse", Confuse);
	matCam.Set("shake", Shake);
	if (!Shake && !Confuse && !Chaos)
	{
		GraphicsAPI::Blit(source, destination, nullptr, cam);
	}
	else
		GraphicsAPI::Blit(source, destination, &matCam, cam);
	return true;
}

AE_REGISTER_COMPONENT(PostEffectCameraBreakout);