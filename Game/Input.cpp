#include "Input.h"
#include "EngineAPIContext.h"

bool Input::GetKey(KeyCode keycode)
{
	const AE_API* ae_api = ENGINEAPICONTEXT::Get();
	if (!ae_api)
		return false;

	return ae_api->getKey(static_cast<int32_t>(keycode));
}

float Input::GetAxis(const char* nameAxis)
{
	const AE_API* ae_api = ENGINEAPICONTEXT::Get();
	if(!ae_api)
		return 0.0f;

	return ae_api->getAxis(nameAxis);
}