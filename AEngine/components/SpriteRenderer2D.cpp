#include "SpriteRenderer2D.h"
#include "../Renderer/GameObject.h"
#include "../Renderer/Material.h"
#include "../components/Transform.h"
#include "../scene/AssetManager.h"

void SpriteRenderer2D::OnInspector()
{
	RegisterField(material);
	RegisterField(texture);
	RegisterField(color);
	SetColor(color);
}
void SpriteRenderer2D::Start()
{
	transform = gameObject->GetComponent<Transform>();
	material = AssetManager::LoadMaterial("standartUIMaterial");
}

AE_REGISTER_COMPONENT(SpriteRenderer2D)