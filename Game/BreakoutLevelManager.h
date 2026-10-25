#pragma once
#include "AComponent.h"
#include "AComponentRef.h"
#include "Transform.h"
#include "BreakoutPlayer.h"
#include "BreakoutBall.h"
#include "PostEffectCameraBreakout.h"
#include "Block.h"
#include "Camera.h"
#include "PowerUp.h"
#include "Texture.h"
#include <string>
#include <vector>

class GameObject;
struct Vector3;

class BreakoutLevel : public AComponent
{
	AE_REGISTER_CLASS(BreakoutLevel);
public:
	enum class Direction
	{
		UP = 0,
		RIGHT = 1,
		DOWN = 2,
		LEFT = 3
	};
	using Collision = std::tuple<bool, Direction, Vector2>;
	Direction VectorDirection(Vector2 target);
	void Load(std::string file, unsigned int levelWidth, unsigned int levelHeight);
	Collision CheckCollision(AComponentRef<Transform>& one, AComponentRef<Transform>& two, AComponentRef<BreakoutBall> player);
	bool CheckCollision(AComponentRef<Transform>& one, AComponentRef<Transform>& two);
	void DoCollisionWithPaddle(AComponentRef<Transform>& player, AComponentRef<Transform>& paddle, AComponentRef<BreakoutBall>& playerComponent);
	void DoCollision(AComponentRef<Transform>& player, AComponentRef<BreakoutBall>& playerComponent);
	void DoCollisionWithPowerUps(AComponentRef<BreakoutBall>& ballPlayer, AComponentRef<Transform>& platformPlayer);
	void SpawnPowerUps(Vector3 pos);
	void ActivatePowerUp(AComponentRef<PowerUp>& powerUp, AComponentRef<BreakoutBall>& playerComponent, AComponentRef<Transform>& platformComponentTransform);
	void ResetLevel();
	void UpdatePowerUps(AComponentRef<BreakoutBall>& playerComponent, AComponentRef<SpriteRenderer2D>& platformComponent, float dt);
	void Update(float deltaTime) override;
private:
	std::vector<AComponentRef<Block>> Bricks;
	void init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight);
	bool IsOtherPowerUpActive(std::string type);
	std::string levelName = "";
	unsigned int SavedlevelHeight = 0;
	unsigned int SavedlevelWidth = 0;
	std::vector<std::pair<AComponentRef<Transform>, AComponentRef<PowerUp>>> powerUps;
	struct PowerUpActivatedInfo
	{
		std::string type = "";
		float Duraction = 0.0f;
		bool Activated = true;
		PowerUpActivatedInfo(std::string _type, float _Duraction, bool _Activated)
		{
			type = _type;
			Duraction = _Duraction;
			Activated = _Activated;
		}
	};
	std::vector<PowerUpActivatedInfo> activatedPowerUps;
	struct TexturesForPowerUps
	{
		Texture speedPU;
		Texture stickyPU;
		Texture pass_throughPU;
		Texture pad_size_increasePU;
		Texture confusePU;
		Texture chaosPU;
	};
	TexturesForPowerUps texturesForPU;
	AComponentRef<PostEffectCameraBreakout> cam;
	GameObject newCam;
	float ShakeTime = 0;
};