#include "BreakoutLevelManager.h"
#include "GameObject.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "SpriteRenderer2D.h"
#include "ResourceManager.h"
#include "BreakoutBall.h"
#include "Camera.h"
#include "Texture.h"
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
bool BreakoutLevel::IsOtherPowerUpActive(std::string type)
{
	for (const auto& powerUp : activatedPowerUps)
	{
		if (powerUp.Activated)
			if (powerUp.type == type)
				return true;
	}
	return false;
}
void BreakoutLevel::UpdatePowerUps(AComponentRef<BreakoutBall>& playerComponent, AComponentRef<SpriteRenderer2D>& platformComponent, float dt)
{
	for (auto& powerUp : powerUps)
	{
		Vector3 pos = powerUp.first->GetPosition();
		powerUp.first->SetPosition(Vector3(pos.x, pos.y - (1.5f * dt), pos.z));
	}
	for (auto& powerUp : activatedPowerUps)
	{
		powerUp.Duraction -= dt;

		if (powerUp.Duraction <= 0.0f)
		{
			powerUp.Activated = false;

			if (powerUp.type == "sticky")
			{
				if (!IsOtherPowerUpActive("sticky"))
				{
					playerComponent->Sticky = false;
					platformComponent->setColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
				}
			}
			else if (powerUp.type == "pass_through")
			{
				if (!IsOtherPowerUpActive("pass_through"))
				{
					playerComponent->PassThrough = false;
					playerComponent->spriteRenderer->setColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
				}
			}
			else if (powerUp.type == "confuse")
			{
				if (!IsOtherPowerUpActive("confuse"))
				{
					cam->Confuse = false;
				}
			}
			else if (powerUp.type == "chaos")
			{
				if (!IsOtherPowerUpActive("chaos"))
				{
					cam->Chaos = false;
				}
			}
		}
	}
	activatedPowerUps.erase(std::remove_if(activatedPowerUps.begin(), activatedPowerUps.end(), [](const PowerUpActivatedInfo& PUAI) {return !PUAI.Activated; }), activatedPowerUps.end());
}
void BreakoutLevel::Update(float deltaTime)
{
	if (ShakeTime > 0.0f)
	{
		ShakeTime -= deltaTime;
		if (ShakeTime <= 0.0f)
			cam->Shake = false;
	}
}
void BreakoutLevel::init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight)
{
	unsigned int height = tileData.size();
	unsigned int width = tileData[0].size();
	float unit_width = static_cast<float>(levelWidth) / static_cast<float>(width);
	float unit_height = static_cast<float>((levelHeight / 2)) / static_cast<float>(height);
	if (!newCam)
	{
		newCam = GameObject::Create("BreakoutCam");
		AComponentRef<Camera> camComponent = newCam.AddAComponent<Camera>();
		camComponent->SetProjection(Camera::ProjectionCameraType::Orthographic);
		cam = newCam.AddAComponent<PostEffectCameraBreakout>();
		AComponentRef<Transform> camTransform = newCam.GetAComponent<Transform>();
		camTransform->SetPosition(Vector3(4.0f, 3.0f, 3.0f));
		texturesForPU.speedPU = ResourceManager::Load<Texture>("Assets/BreakoutGame/powerup_speed.png");
		texturesForPU.stickyPU = ResourceManager::Load<Texture>("Assets/BreakoutGame/powerup_sticky.png");
		texturesForPU.pass_throughPU = ResourceManager::Load<Texture>("Assets/BreakoutGame/powerup_passthrough.png");
		texturesForPU.pad_size_increasePU = ResourceManager::Load<Texture>("Assets/BreakoutGame/powerup_increase.png");
		texturesForPU.confusePU = ResourceManager::Load<Texture>("Assets/BreakoutGame/powerup_confuse.png");
		texturesForPU.chaosPU = ResourceManager::Load<Texture>("Assets/BreakoutGame/powerup_chaos.png");
	}
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
}
bool BreakoutLevel::CheckCollision(AComponentRef<Transform>& one, AComponentRef<Transform>& two)
{
	Vector3 onePos = one->GetPosition();
	Vector3 oneScale = one->GetScale();
	Vector3 twoPos = two->GetPosition();
	Vector3 twoScale = two->GetScale();
	bool collisionX = (onePos.x + (oneScale.x / 2)) >= twoPos.x - (twoScale.x / 2) && twoPos.x + (twoScale.x / 2) >= onePos.x - (oneScale.x / 2);
	bool collisionY = (onePos.y + (oneScale.y / 2)) >= twoPos.y - (twoScale.y / 2) && twoPos.y + (twoScale.y / 2) >= onePos.y - (oneScale.y / 2);

	return collisionX && collisionY;
}
void BreakoutLevel::SpawnPowerUps(Vector3 pos)
{
	std::cout << "fsdjkflsfkl\n";
	auto _rand = [](unsigned int chance) {unsigned int random = rand() % chance; std::cout << random << "\n"; return random == 0; };
	if (_rand(25))
	{
		std::cout << "[poiuyy\n";
		GameObject newPowerUp = GameObject::Create("SpeedPowerUp");
		newPowerUp.GetAComponent<Transform>()->SetPosition(pos);
		newPowerUp.GetAComponent<Transform>()->SetScale(Vector3(0.6f, 0.2f, 1.0f));
		newPowerUp.AddAComponent<PowerUp>("speed", 0.0f);
		AComponentRef<SpriteRenderer2D> newPowerUpComponent = newPowerUp.AddAComponent<SpriteRenderer2D>();
		newPowerUpComponent->setTexture(texturesForPU.speedPU);
		newPowerUpComponent->setColor(Vector4(0.5f, 0.5f, 1.0f, 1.0f));
		powerUps.push_back({ newPowerUp.GetAComponent<Transform>(), newPowerUp.GetAComponent<PowerUp>() });
	}
	if (_rand(25))
	{
		std::cout << "[poiuyy\n";
		GameObject newPowerUp = GameObject::Create("StickyPowerUp");
		newPowerUp.GetAComponent<Transform>()->SetPosition(pos);
		newPowerUp.GetAComponent<Transform>()->SetScale(Vector3(0.6f, 0.2f, 1.0f));
		newPowerUp.AddAComponent<PowerUp>("sticky", 20.0f);
		AComponentRef<SpriteRenderer2D> newPowerUpComponent = newPowerUp.AddAComponent<SpriteRenderer2D>();
		newPowerUpComponent->setTexture(texturesForPU.stickyPU);
		newPowerUpComponent->setColor(Vector4(1.0f, 0.5f, 1.0f, 1.0f));
		powerUps.push_back({ newPowerUp.GetAComponent<Transform>(), newPowerUp.GetAComponent<PowerUp>() });
	}
	if (_rand(25))
	{
		std::cout << "[poiuyy\n";
		GameObject newPowerUp = GameObject::Create("Pass_throughPowerUP");
		newPowerUp.GetAComponent<Transform>()->SetPosition(pos);
		newPowerUp.GetAComponent<Transform>()->SetScale(Vector3(0.6f, 0.2f, 1.0f));
		newPowerUp.AddAComponent<PowerUp>("pass_through", 10.0f);
		AComponentRef<SpriteRenderer2D> newPowerUpComponent = newPowerUp.AddAComponent<SpriteRenderer2D>();
		newPowerUpComponent->setTexture(texturesForPU.pass_throughPU);
		newPowerUpComponent->setColor(Vector4(0.5f, 1.0f, 0.5f, 1.0f));
		powerUps.push_back({ newPowerUp.GetAComponent<Transform>(), newPowerUp.GetAComponent<PowerUp>() });
	}
	if (_rand(25))
	{
		std::cout << "[poiuyy\n";
		GameObject newPowerUp = GameObject::Create("Pad_size_increasePowerUp");
		newPowerUp.GetAComponent<Transform>()->SetPosition(pos);
		newPowerUp.GetAComponent<Transform>()->SetScale(Vector3(0.6f, 0.2f, 1.0f));
		newPowerUp.AddAComponent<PowerUp>("pad_size_increase", 0.0f);
		AComponentRef<SpriteRenderer2D> newPowerUpComponent = newPowerUp.AddAComponent<SpriteRenderer2D>();
		newPowerUpComponent->setTexture(texturesForPU.pad_size_increasePU);
		newPowerUpComponent->setColor(Vector4(1.0f, 0.6f, 0.4f, 1.0f));
		powerUps.push_back({ newPowerUp.GetAComponent<Transform>(), newPowerUp.GetAComponent<PowerUp>() });
	}
	if (_rand(15))
	{
		GameObject newPowerUp = GameObject::Create("ConfusePowerUp");
		newPowerUp.GetAComponent<Transform>()->SetPosition(pos);
		newPowerUp.GetAComponent<Transform>()->SetScale(Vector3(0.6f, 0.2f, 1.0f));
		newPowerUp.AddAComponent<PowerUp>("confuse", 15.0f);
		AComponentRef<SpriteRenderer2D> newPowerUpComponent = newPowerUp.AddAComponent<SpriteRenderer2D>();
		newPowerUpComponent->setTexture(texturesForPU.confusePU);
		newPowerUpComponent->setColor(Vector4(1.0f, 0.3f, 0.3f, 1.0f));
		powerUps.push_back({ newPowerUp.GetAComponent<Transform>(), newPowerUp.GetAComponent<PowerUp>() });
	}
	if (_rand(15))
	{
		GameObject newPowerUp = GameObject::Create("ChaosPowerUp");
		newPowerUp.GetAComponent<Transform>()->SetPosition(pos);
		newPowerUp.GetAComponent<Transform>()->SetScale(Vector3(0.6f, 0.2f, 1.0f));
		newPowerUp.AddAComponent<PowerUp>("chaos", 15.0f);
		AComponentRef<SpriteRenderer2D> newPowerUpComponent = newPowerUp.AddAComponent<SpriteRenderer2D>();
		newPowerUpComponent->setTexture(texturesForPU.chaosPU);
		newPowerUpComponent->setColor(Vector4(0.9f, 0.25f, 0.25f, 1.0f));
		powerUps.push_back({ newPowerUp.GetAComponent<Transform>(), newPowerUp.GetAComponent<PowerUp>() });
	}
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
void BreakoutLevel::ActivatePowerUp(AComponentRef<PowerUp>& powerUp, AComponentRef<BreakoutBall>& playerComponent, AComponentRef<Transform>& platformComponentTransform)
{
	activatedPowerUps.emplace_back(powerUp->Type, powerUp->Duraction, true);
	if (powerUp->Type == "speed")
	{
		playerComponent->Velocity.x *= 1.2f;
		playerComponent->Velocity.y *= 1.2f;
	}
	else if (powerUp->Type == "sticky")
	{
		playerComponent->Sticky = true;
		platformComponentTransform->GetGameObject().GetAComponent<SpriteRenderer2D>()->setColor(Vector4(1.0f, 0.5f, 1.0f, 1.0f));
	}
	else if (powerUp->Type == "pass_through")
	{
		playerComponent->PassThrough = true;
		playerComponent->spriteRenderer->setColor(Vector4(1.0f, 0.5f, 0.5f, 1.0f));
	}
	else if (powerUp->Type == "pad_size_increase")
	{
		platformComponentTransform->SetScale(Vector3(platformComponentTransform->GetScale().x + 0.5f, platformComponentTransform->GetScale().y, platformComponentTransform->GetScale().z));
	}
	else if (powerUp->Type == "confuse")
	{
		if (!cam->Chaos)
			cam->Confuse = true;
	}
	else if (powerUp->Type == "chaos")
	{
		if (!cam->Confuse)
			cam->Chaos = true;
	}
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
			if (!(playerComponent->PassThrough && !box->isSolid))
			{
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
			}
			if (!box->isSolid)
			{
				box->isDestroy = true;
				SpawnPowerUps(box->transform->GetPosition());
				box->GetGameObject().Destroy();

				it = Bricks.erase(it);
				std::cout << "COLLISION\n";
				continue;
			}
			else
			{
				cam->Shake = true;
				ShakeTime = 0.05f;
			}
		}
		++it;
	}
}
void BreakoutLevel::DoCollisionWithPowerUps(AComponentRef<BreakoutBall>& ballPlayer, AComponentRef<Transform>& platformPlayer)
{
	for (auto it = powerUps.begin(); it != powerUps.end();)
	{
		AComponentRef<Transform> itTransform = (*it).first;
		AComponentRef<PowerUp> itPowerUp = (*it).second;
		if (itTransform)
		{
			if (itTransform->GetPosition().y <= 0.0f)
			{
				itTransform->GetGameObject().Destroy();
				it = powerUps.erase(it);
				continue;
			}
			if (CheckCollision(platformPlayer, itTransform))
			{
				ActivatePowerUp(itPowerUp, ballPlayer, platformPlayer );
				itTransform->GetGameObject().Destroy();
				it = powerUps.erase(it);
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

		playerComponent->Stuck = playerComponent->Sticky;
	}
}

void BreakoutLevel::ResetLevel()
{
	for (AComponentRef<Block>& block : Bricks)
	{
		if (block)
			block->GetGameObject().Destroy();
	}
	for (auto& powerUp : powerUps)
	{
		if (powerUp.first)
			powerUp.first->GetGameObject().Destroy();
	}
	cam->Confuse = false;
	cam->Chaos = false;
	cam->Shake = false;
	powerUps.clear();
	activatedPowerUps.clear();
	Bricks.clear();
	Load(levelName, SavedlevelWidth, SavedlevelHeight);
}

AE_REGISTER_COMPONENT(BreakoutLevel);