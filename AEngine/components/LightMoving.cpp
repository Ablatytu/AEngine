#include "LightMoving.h"
#include "../Renderer/GameObject.h"
void LightMoving::Start()
{
	transform = gameObject->GetComponent<Transform>();
}
void LightMoving::Update()
{
	if (CanMoving)
	{
		if (CanUp)
		{
			if (up)
			{
				transform->position.y += 0.001f;
				if (transform->position.y > 5.0f)
					up = false;
			}
			else
			{
				transform->position.y -= 0.001f;
				if (transform->position.y < -1.0f)
					up = true;
			}
		}
		else
		{
			if (rigntPos)
			{
				transform->position.x -= 0.001f;
				if (transform->position.x < -5.0f)
					rigntPos = false;
			}
			else
			{
				transform->position.x += 0.001f;
				if (transform->position.x > 5.0f)
					rigntPos = true;
			}
		}
		transform->UpdateMatrices();
	}
}
void LightMoving::OnInspector()
{
	RegisterField(rigntPos);
	RegisterField(CanMoving);
}