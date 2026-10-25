#include "Camera.h"
#include "EngineAPIContext.h"

void Camera::SetProjection(ProjectionCameraType typeProj)
{
	const AE_API* ae_api = ENGINEAPICONTEXT::Get();
	if (!ae_api)
		return;

	ae_api->setCameraProjection(ae_component_handle, static_cast<uint8_t>(typeProj));
}