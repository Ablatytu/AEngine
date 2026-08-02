#include "Light.h"
#include "../Renderer/GameObject.h"
#include "Transform.h"
#include "../scene/SceneManager.h"
#include "../scene/LightManager.h"
#include <glad/glad.h>
#include <iostream>
void Light::Awake()
{
	transform = gameObject->GetComponent<Transform>();
	SceneManager::Get().GetActiveScene().A_LightManager->AddLight(this);
}
void Light::OnInspector()
{
	RegisterField(intensive);
	RegisterField(Radius);
	RegisterField(lightColor);
	RegisterField(typeLight);
	RegisterField(shadow);
	Range(intensive, 0.0f, 5.0f);
	SetColor(lightColor);
}
Light::~Light()
{
	if (shadowMap.created)
	{
		std::cout << "Deleting shadowmap\n";
		glDeleteFramebuffers(1, &shadowMap.IDfbo);
		glDeleteTextures(1, &shadowMap.ID);

		shadowMap.created = false;
	}
}
void Light::OnDestroy()
{
	SceneManager::Get().GetActiveScene().A_LightManager->RemoveLight(this);
}