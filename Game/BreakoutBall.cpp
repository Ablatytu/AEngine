#include "BreakoutBall.h"
#include "GameObject.h"
#include "ParticleSystem.h"

void BreakoutBall::Start()
{
	transform = GetGameObject().GetAComponent<Transform>();
}

Vector2 BreakoutBall::Move(float dt, unsigned int width, unsigned int height)
{
	if (!Stuck)
	{
		Vector3 pos = transform->GetPosition();
		pos.x += Velocity.x * dt;
		pos.y += Velocity.y * dt;
		transform->SetPosition(pos);
		if (pos.x - Radius <= 0.0f)
		{
			Velocity.x = -Velocity.x;
			transform->SetPosition(Vector3(Radius, pos.y, pos.z));
		}
		else if (pos.x + Radius >= width)
		{
			Velocity.x = -Velocity.x;
			transform->SetPosition(Vector3(width - Radius, pos.y, pos.z));
		}
		if (pos.y + Radius >= height)
		{
			Velocity.y = -Velocity.y;
			transform->SetPosition(Vector3(pos.x, height - Radius, pos.z));
		}
	}
	return Vector2(transform->GetPosition().x, transform->GetPosition().y);
}
bool BreakoutBall::CheckReset()
{
	return transform->GetPosition().y + Radius <= 0.0f;
}
void BreakoutBall::Reset(Vector2 posPlatform, Vector2 scalePlatform)
{
	Stuck = true;
	transform->SetPosition(Vector3(posPlatform.x, posPlatform.y + (scalePlatform.y / 2) + (Radius), transform->GetPosition().z));
	transform->SetScale(Vector3(Radius * 2, Radius * 2, 1.0f));
}


AE_REGISTER_COMPONENT(BreakoutBall)