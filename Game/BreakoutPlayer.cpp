#include "BreakoutPlayer.h"
#include "Input.h"
#include "Vector3.h"
#include "SpriteRenderer2D.h"
#include "ResourceManager.h"
#include "UIContoller.h"
#include "BreakoutLevelManager.h"
#include "ParticleSystem.h"
#include "Material.h"

void BreakoutPlayer::Start()
{
	transform = GetGameObject().GetAComponent<Transform>();
	GameObject ball = GameObject::Create("Ball");
	AComponentRef<SpriteRenderer2D> ballSprite = ball.AddAComponent<SpriteRenderer2D>();
	ballSprite->setTexture(ResourceManager::Load<Texture>("Assets/BreakoutGame/BreakoutPlayerTalisman.png"));
	ballPlayer = ball.AddAComponent<BreakoutBall>();
	AComponentRef<Transform> ballTransform = ball.GetAComponent<Transform>();
	Vector3 pos = transform->GetPosition();
	ballTransform->SetPosition(Vector3(pos.x, pos.y + (transform->GetScale().y / 2) + (ballPlayer->Radius), pos.z));
	ballTransform->SetScale(Vector3(ballPlayer->Radius * 2, ballPlayer->Radius * 2, 1.0f));
	AComponentRef<ParticleSystem> BallParticle = ball.AddAComponent<ParticleSystem>();
	Material ParticleMaterial = ResourceManager::Load<Material>("standartParticleMaterial");
	Texture ParticleTexture = ResourceManager::Load<Texture>("Assets/BreakoutGame/particle.png");
	ParticleMaterial.SetTexture("particleTexture", ParticleTexture);
	BallParticle->SetMaterial(ParticleMaterial);

}
void BreakoutPlayer::Update(float deltaTime)
{
	float finalSpeed = speed * deltaTime;
	if (Input::GetKey(KeyCode::A))
	{
		Vector3 position = transform->GetPosition();
		if (position.x >= 0.5f)
		{
			transform->SetPosition(Vector3(position.x - finalSpeed, position.y, position.z));
			if (ballPlayer->Stuck)
			{
				Vector3 positionBall = ballPlayer->transform->GetPosition();
				ballPlayer->transform->SetPosition(Vector3(positionBall.x - finalSpeed, positionBall.y, positionBall.z));
			}
		}
	}
	else if(Input::GetKey(KeyCode::D))
	{
		Vector3 position = transform->GetPosition();
		if (position.x <= 7.5f)
		{
			transform->SetPosition(Vector3(position.x + finalSpeed, position.y, position.z));
			if (ballPlayer->Stuck)
			{
				Vector3 positionBall = ballPlayer->transform->GetPosition();
				ballPlayer->transform->SetPosition(Vector3(positionBall.x + finalSpeed, positionBall.y, positionBall.z));
			}
		}
		
	}
	if (Input::GetKey(KeyCode::Space))
		ballPlayer->Stuck = false;
	ballPlayer->Move(deltaTime, 8, 6);
	if (ballPlayer->CheckReset())
	{
		uiController->Levels[0]->ResetLevel();
		Vector3 posPlatform = transform->GetPosition();
		Vector3 scalePlatform = transform->GetScale();
		ballPlayer->Reset(Vector2(posPlatform.x, posPlatform.y), Vector2(scalePlatform.x, scalePlatform.y));
		return;
	}
	uiController->Levels[0]->DoCollision(ballPlayer->transform, ballPlayer);
	uiController->Levels[0]->DoCollisionWithPaddle(ballPlayer->transform, transform, ballPlayer);
}

void BreakoutPlayer::OnInspector()
{
	AE_REGISTER_FIELD(speed);
	AE_SET_RANGE(speed, 0.0f, 50.0f);
}

AE_REGISTER_COMPONENT(BreakoutPlayer);