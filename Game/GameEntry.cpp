#include "../GameAPI/GameABI.h"
#include "EngineAPIContext.h"
#include "ComponentStorage.h"
#include "TestComponent.h"
#include "Camera.h"
#include "GameObject.h"
#include "ComponentSystem.h"
#include "RenderImage.h"

static void AE_CALL GameUpdate(float deltaTime)
{
	ComponentStorage::Get().UpdateComponents(deltaTime);
}
static void AE_CALL GameStart()
{
	ComponentStorage::Get().StartComponents();
}
static bool AE_CALL GameOnRenderImage(AE_GAME_RESOURCES_INFO::AE_RENDERIMAGE source, AE_GAME_RESOURCES_INFO::AE_RENDERIMAGE destionation, AE_COMPONENT_HANDLE cam, AE_GAMEOBJECT_HANDLE ae_gameobject_handle)
{
	return ComponentStorage::Get().OnRenderImageComponents(RenderImage(source), RenderImage(destionation), cam, ae_gameobject_handle);

}
static void AE_CALL GameShutdown()
{
	const AE_API* api = ENGINEAPICONTEXT::Get();
	if (api && api->log)
		ENGINEAPICONTEXT::Get()->log("Bye, bitch");
	ComponentStorage::Get().RemoveAllGameComponents();
	ComponentSystem::Get().RemoveAllGameAComponents();
	ENGINEAPICONTEXT::UnBind();
}

static const char* AE_CALL GameGetRegisteredAComponentName(unsigned int ID)
{
	return ComponentSystem::Get().GetAComponentName(ID);
}
static const int AE_CALL GameGetRegisteredAComponentsCount()
{
	return ComponentSystem::Get().GetAComponentCount();
}
static void AE_CALL GameAddAComponent(unsigned int ID, AE_GAMEOBJECT_HANDLE ae_gameObject_handle)
{
	ComponentSystem::Get().AddAComponent(ID, ae_gameObject_handle);
}
static void AE_CALL GameGetAComponentFromGameObject(AE_GAMEOBJECT_HANDLE ae_gameObject_handle, AE_COMPONENT_INFO* ae_component_handle, int size)
{
	ComponentStorage::Get().ReturnComponentInfoFromGameObject(ae_gameObject_handle, ae_component_handle, size);
}
static int AE_CALL GameGetAComponentsCountFromGameObject(AE_GAMEOBJECT_HANDLE ae_gameObject_handle)
{
	return ComponentStorage::Get().ReturnComponentCountFromGameObject(ae_gameObject_handle);
}
static void AE_CALL GameGetAComponentsInfo(AE_COMPONENT_INFO* ae_component_info_storage, int size)
{
	ComponentStorage::Get().ReturnAllAComponentsInfo(ae_component_info_storage, size);
}
static void AE_CALL GameRemoveAComponent(AE_GAMEOBJECT_HANDLE ae_gameobject_handle, AE_COMPONENT_HANDLE ae_component_handle)
{
	if(!AE_IsNull(ae_component_handle))
		ComponentStorage::Get().Remove(ae_gameobject_handle, ae_component_handle);
}
static int AE_CALL GameGetAComponentCount()
{
	return ComponentStorage::Get().GetAComponentCount();
}
static void AE_CALL GameRemoveAllAComponents()
{
	ComponentStorage::Get().RemoveAllGameComponents();
}
static void AE_CALL GameRemoveAComponentsFromGameObject(AE_GAMEOBJECT_HANDLE ae_gameobject_handle)
{
	ComponentStorage::Get().RemoveAllFromGameObject(ae_gameobject_handle);
}

AE_EXPORT bool AE_CALL AE_GAME_API(const AE_API* AEngineAPI, AE_GAMEAPI* GameAPI)
{
	if (!AEngineAPI || !GameAPI)
		return false;
	if (!AEngineAPI->log)
		return false;

	ENGINEAPICONTEXT::Bind(AEngineAPI);

	*GameAPI = {};

	GameAPI->shutdown = &GameShutdown;
	GameAPI->update = &GameUpdate;
	GameAPI->start = &GameStart;
	GameAPI->getRegisteredAComponentName = &GameGetRegisteredAComponentName;
	GameAPI->getRegisteredAComponentsCount = &GameGetRegisteredAComponentsCount;
	GameAPI->getAComponentsCountFromGameObject = &GameGetAComponentsCountFromGameObject;
	GameAPI->getAComponentsFromGameObject = &GameGetAComponentFromGameObject;
	GameAPI->getAComponentsInfo = &GameGetAComponentsInfo;
	GameAPI->getAComponentCount = &GameGetAComponentCount;
	GameAPI->addAComponent = &GameAddAComponent;
	GameAPI->clearGameAComponents = &GameRemoveAllAComponents;
	GameAPI->removeAComponent = &GameRemoveAComponent;
	GameAPI->removeAComponentsFromGameObject = &GameRemoveAComponentsFromGameObject;
	GameAPI->onRenderImage = &GameOnRenderImage;

	ENGINEAPICONTEXT::Get()->log("Hello, bitch");
	return true;
}