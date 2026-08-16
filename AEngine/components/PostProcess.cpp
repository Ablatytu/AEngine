#include "../Renderer/GameObject.h"
#include "PostProcess.h"
#include "../scene/SceneManager.h"
#include "../components/Camera.h"
#include "../scene/Scene.h"
void PostProcess::OnInspector()
{
	RegisterField(enabled);
	RegisterField(exposure);
	RegisterField(minExposure);
	RegisterField(maxExposure);
	RegisterField(adaptationSpeed);
	RegisterField(AutoExposure);
	RegisterField(Bloom);
	RegisterField(SSAO);

	Range(exposure, 0.01f, 50.0f);
	Range(minExposure, 0.01f, 50.0f);
	Range(maxExposure, 0.01f, 50.0f);
	Range(adaptationSpeed, 0.01f, 10.0f);
}
void PostProcess::Awake()
{
	if (gameObject->GetComponent<Camera>())
		camera = gameObject->GetComponent<Camera>();
	SceneManager::Get().GetActiveScene().postProcess = this;
}
void PostProcess::OnDestroy()
{
	if(this == SceneManager::Get().GetActiveScene().postProcess)
		SceneManager::Get().GetActiveScene().postProcess = nullptr;
}

AE_REGISTER_COMPONENT(PostProcess)