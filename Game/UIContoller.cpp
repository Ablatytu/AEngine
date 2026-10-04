#include "UIContoller.h"
#include "GameObject.h"
#include "SpriteRenderer2D.h"
#include "ResourceManager.h"
#include "BreakoutPlayer.h"
#include "BreakoutLevelManager.h"

void UIController::Start()
{
	GameObject Background = GameObject::Create("Background");
	AComponentRef<SpriteRenderer2D> BackgroundSprite = Background.AddAComponent<SpriteRenderer2D>();
	BackgroundSprite->setTexture(ResourceManager::Load<Texture>("Assets/BreakoutGame/background.jpg"));
	AComponentRef<Transform> BackgroundTransform = Background.GetAComponent<Transform>();
	BackgroundTransform->SetPosition(Vector3(4.0f, 3.0f, -1.0f));
	BackgroundTransform->SetScale(Vector3(8.0f, 6.0f, 1.0f));
	AComponentRef<BreakoutLevel> level1 = GetGameObject().AddAComponent<BreakoutLevel>();
	level1->Load("Assets/BreakoutGame/Levels/Level1.txt", 8, 6);
	Levels.push_back(level1);
	GameObject Player = GameObject::Create("BreakoutPlayer");
	AComponentRef<BreakoutPlayer> playerController = Player.AddAComponent<BreakoutPlayer>();
	playerController->uiController = GetGameObject().GetAComponent<UIController>();
	AComponentRef<SpriteRenderer2D> PlayerSprite = Player.AddAComponent<SpriteRenderer2D>();
	PlayerSprite->setTexture(ResourceManager::Load<Texture>("Assets/BreakoutGame/paddle.png"));
	AComponentRef<Transform> PlayerTransform = Player.GetAComponent<Transform>();
	PlayerTransform->SetPosition(Vector3(3.5f, 0.2f, -0.2f));
	PlayerTransform->SetScale(Vector3(1.0f, 0.2f, 1.0f));
}

AE_REGISTER_COMPONENT(UIController);