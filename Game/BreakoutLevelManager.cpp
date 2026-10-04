#include "BreakoutLevelManager.h"
#include "GameObject.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "SpriteRenderer2D.h"
#include "ResourceManager.h"
#include "BreakoutBall.h"
#include <fstream>
#include <sstream>


void BreakoutLevel::Load(std::string file, unsigned int levelWidth, unsigned int levelHeight)
{
	Bricks.clear();

	unsigned int tileCode = 0;
	std::string line = "";
	std::ifstream fstream(file);
	std::vector<std::vector<unsigned int>> tileData;
	if (fstream)
	{
		while (std::getline(fstream, line))
		{
			std::istringstream sstream(line);
			std::vector<unsigned int> row;
			while (sstream >> tileCode)
			{
				row.push_back(tileCode);
			}
			tileData.push_back(row);
		}
		if (tileData.size() > 0)
			init(tileData, levelWidth, levelHeight);
	}
	levelName = file;
	SavedlevelHeight = levelHeight;
	SavedlevelWidth = levelWidth;
}
void BreakoutLevel::init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight)
{
	unsigned int height = tileData.size();
	unsigned int width = tileData[0].size();
	float unit_width = static_cast<float>(levelWidth) / static_cast<float>(width);
	float unit_height = static_cast<float>((levelHeight / 2)) / static_cast<float>(height);
	for (unsigned int y = 0; y < height; ++y)
	{
		for (unsigned int x = 0; x < width; ++x)
		{
			if (tileData[y][x] == 1)
			{
				Vector2 pos((unit_width * x) + (unit_width * 0.5f), levelHeight - ((unit_height * y) + (unit_height * 0.5f)));
				Vector2 size(unit_width, unit_height);
				std::string name = std::string("Block(") + std::to_string(x) + std::string(":") + std::to_string(y) + std::string(")");
				GameObject block = GameObject::Create(name.c_str());
				AComponentRef<SpriteRenderer2D> spriteComponent = block.AddAComponent<SpriteRenderer2D>();
				spriteComponent->setTexture(ResourceManager::Load<Texture>("Assets/BreakoutGame/block_solid.png"));
				AComponentRef<Block> blockComponent = block.AddAComponent<Block>();
				blockComponent->isSolid = true;
				AComponentRef<Transform> transformComponent = block.GetAComponent<Transform>();
				transformComponent->SetPosition(Vector3(pos.x, pos.y, 1.0f));
				transformComponent->SetScale(Vector3(size.x, size.y, 1.0f));
				blockComponent->transform = transformComponent;
				Bricks.push_back(blockComponent);
			}
			else if (tileData[y][x] > 1)
			{
				Vector3 color = Vector3(1.0f, 1.0f, 1.0f);
				if (tileData[y][x] == 2)
					color = Vector3(0.2f, 0.6f, 1.0f);
				else if(tileData[y][x] == 3)
					color = Vector3(0.0f, 0.7f, 0.0f);
				else if (tileData[y][x] == 4)
					color = Vector3(0.8f, 0.8f, 0.4f);
				else if (tileData[y][x] == 5)
					color = Vector3(1.0f, 0.5f, 0.0f);

				Vector2 pos((unit_width * x) + (unit_width * 0.5f), levelHeight - ((unit_height * y) + (unit_height * 0.5f)));
				Vector2 size(unit_width, unit_height);
				std::string name = std::string("Block(") + std::to_string(x) + std::string(":") + std::to_string(y) + std::string(")");
				GameObject block = GameObject::Create(name.c_str());
				AComponentRef<SpriteRenderer2D> spriteComponent = block.AddAComponent<SpriteRenderer2D>();
				spriteComponent->setTexture(ResourceManager::Load<Texture>("Assets/BreakoutGame/block.png"));
				spriteComponent->setColor(Vector4(color.x, color.y, color.z, 1.0f));
				AComponentRef<Block> blockComponent = block.AddAComponent<Block>();
				AComponentRef<Transform> transformComponent = block.GetAComponent<Transform>();
				transformComponent->SetPosition(Vector3(pos.x, pos.y, 0.0f));
				transformComponent->SetScale(Vector3(size.x, size.y, 1.0f));
				blockComponent->transform = transformComponent;
				Bricks.push_back(blockComponent);
			}
		}
	}
}

BreakoutLevel::Collision BreakoutLevel::CheckCollision(AComponentRef<Transform>& one, AComponentRef<Transform>& two, AComponentRef<BreakoutBall> player)
{
	Vector3 centerPos = one->GetPosition();
	Vector3 centerPosTwo = two->GetPosition();
	Vector3 scaleTwo = two->GetScale();
	Vector3 HalfscaleTwo = Vector3(scaleTwo.x / 2.0f, scaleTwo.y / 2.0f, scaleTwo.z);

	Vector2 difference = Vector2(centerPos.x - centerPosTwo.x, centerPos.y - centerPosTwo.y);

	float clampedDifferenceX = std::max(-HalfscaleTwo.x, std::min(difference.x, HalfscaleTwo.x));
	float clampedDifferenceY = std::max(-HalfscaleTwo.y, std::min(difference.y, HalfscaleTwo.y));

	Vector2 closest = Vector2(centerPosTwo.x + clampedDifferenceX, centerPosTwo.y + clampedDifferenceY);

	difference = Vector2(closest.x - centerPos.x, closest.y - centerPos.y);

	if (difference.length() < player->Radius)
		return std::make_tuple(true, VectorDirection(difference), difference);
	else
		return std::make_tuple(false, BreakoutLevel::Direction::UP, Vector2(0.0f, 0.0f));
	//Vector3 onePos = one->GetPosition();
	//Vector3 oneScale = one->GetScale();
	//Vector3 twoPos = two->GetPosition();
	//Vector3 twoScale = two->GetScale();
	//bool collisionX = (onePos.x + (oneScale.x / 2)) >= twoPos.x - (twoScale.x / 2) && twoPos.x + (twoScale.x / 2) >= onePos.x - (oneScale.x / 2);
	//bool collisionY = (onePos.y + (oneScale.y / 2)) >= twoPos.y - (twoScale.y / 2) && twoPos.y + (twoScale.y / 2) >= onePos.y - (oneScale.y / 2);

	//return collisionX && collisionY;
}
BreakoutLevel::Direction BreakoutLevel::VectorDirection(Vector2 target)
{
	Vector2 compass[] = {
		Vector2(0.0f, 1.0f),
		Vector2(1.0f, 0.0f),
		Vector2(0.0f, -1.0f),
		Vector2(-1.0f, 0.0f)
	};
	float max = 0.0f;
	unsigned int best_match = -1;
	for (unsigned int i = 0; i < 4; i++)
	{
		float dot_product = target.normalize().dot(compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}

void BreakoutLevel::DoCollision(AComponentRef<Transform>& playerTransform, AComponentRef<BreakoutBall>& playerComponent)
{

	for (auto it = Bricks.begin(); it != Bricks.end(); )
	{
		AComponentRef<Block>& box = *it;
		if (!box)
		{
			it = Bricks.erase(it);
			continue;
		}
		Collision collision = CheckCollision(playerTransform, box->transform, playerComponent);
		if (std::get<0>(collision))
		{
			Direction dir = std::get<1>(collision);
			Vector2 diff_vector = std::get<2>(collision);
			Vector3 posPlayer = playerTransform->GetPosition();
			if (dir == Direction::LEFT || dir == Direction::RIGHT)
			{
				playerComponent->Velocity.x = -playerComponent->Velocity.x;

				float penetration = playerComponent->Radius - std::abs(diff_vector.x);
				if (dir == Direction::LEFT)
					playerTransform->SetPosition(Vector3(posPlayer.x + penetration, posPlayer.y, posPlayer.z));
				else
					playerTransform->SetPosition(Vector3(posPlayer.x - penetration, posPlayer.y, posPlayer.z));
			}
			else
			{
				playerComponent->Velocity.y = -playerComponent->Velocity.y;

				float penetration = playerComponent->Radius - std::abs(diff_vector.y);
				if (dir == Direction::UP)
					playerTransform->SetPosition(Vector3(posPlayer.x, posPlayer.y - penetration, posPlayer.z));
				else
					playerTransform->SetPosition(Vector3(posPlayer.x, posPlayer.y + penetration, posPlayer.z));
			}
			if (!box->isSolid)
			{
				box->isDestroy = true;
				box->GetGameObject().Destroy();

				it = Bricks.erase(it);
				std::cout << "COLLISION\n";
				continue;
			}
		}
		++it;
	}
}

void BreakoutLevel::DoCollisionWithPaddle(AComponentRef<Transform>& player, AComponentRef<Transform>& paddle, AComponentRef<BreakoutBall>& playerComponent)
{
	Collision result = CheckCollision(player, paddle, playerComponent);
	if (!playerComponent->Stuck && std::get<0>(result))
	{
		Vector3 PlayerPos = player->GetPosition();
		Vector3 PaddlePos = paddle->GetPosition();
		float centerBoard = PaddlePos.x;
		float distance = PlayerPos.x - centerBoard;
		float percentage = distance / (paddle->GetScale().x / 2.0f);

		float strength = 2.0f;
		Vector2 oldVelocity = playerComponent->Velocity;
		playerComponent->Velocity.x = 1.0f * percentage * strength;
		playerComponent->Velocity.y = abs(playerComponent->Velocity.y);
		Vector2 normalizeVelocity = playerComponent->Velocity.normalize();
		float lengthOldVelocity = oldVelocity.length();
		playerComponent->Velocity = Vector2(normalizeVelocity.x * lengthOldVelocity, normalizeVelocity.y * lengthOldVelocity);
	}
}

void BreakoutLevel::ResetLevel()
{
	for (AComponentRef<Block>& block : Bricks)
	{
		if (block)
			block->GetGameObject().Destroy();
	}
	Bricks.clear();
	Load(levelName, SavedlevelWidth, SavedlevelHeight);
}

AE_REGISTER_COMPONENT(BreakoutLevel);