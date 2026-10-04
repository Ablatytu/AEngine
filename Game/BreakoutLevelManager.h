#pragma once
#include "AComponent.h"
#include "AComponentRef.h"
#include "Transform.h"
#include "BreakoutPlayer.h"
#include "BreakoutBall.h"
#include "Block.h"
#include <string>
#include <vector>

class GameObject;

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
	void DoCollisionWithPaddle(AComponentRef<Transform>& player, AComponentRef<Transform>& paddle, AComponentRef<BreakoutBall>& playerComponent);
	void DoCollision(AComponentRef<Transform>& player, AComponentRef<BreakoutBall>& playerComponent);
	void ResetLevel();
private:
	std::vector<AComponentRef<Block>> Bricks;
	void init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight);
	std::string levelName = "";
	unsigned int SavedlevelHeight = 0;
	unsigned int SavedlevelWidth = 0;

};