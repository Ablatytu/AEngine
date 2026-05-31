#include "Light.h"
#include "../Renderer/GameObject.h"
#include "Transform.h"
#include "../scene/SceneManager.h"
#include "../scene/LightManager.h"
void Light::Awake()
{
	transform = gameObject->GetComponent<Transform>();
	SceneManager::Get().GetActiveScene().A_LightManager->AddLight(this);
}
void Light::OnInspector()
{
	RegisterField(intensive);
	Range(intensive, 0.0f, 5.0f);
}
void Light::OnDestroy()
{
	SceneManager::Get().GetActiveScene().A_LightManager->RemoveLight(this);
}