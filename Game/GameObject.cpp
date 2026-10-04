#include "GameObject.h"
#include "EngineAPIContext.h"

void GameObject::Destroy()
{
	const AE_API* api = ENGINEAPICONTEXT::Get();
	if (!api)
		return;

	api->destroyGameObject(handle);
}