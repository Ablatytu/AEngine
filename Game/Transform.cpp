#include "Transform.h"

#include "EngineAPIContext.h"

Vector3 Transform::GetPosition() const
{
	const AE_API* api = ENGINEAPICONTEXT::Get();

	AE_VEC3 rawPosition{};

	if (!api || !api->getTransformPosition(ae_component_handle, &rawPosition))
		return {};

	return Vector3{ rawPosition.x, rawPosition.y, rawPosition.z };
}
Vector3 Transform::GetRotation() const
{
	const AE_API* api = ENGINEAPICONTEXT::Get();

	AE_VEC3 rawRotation{};

	if (!api || !api->getTransformRotation(ae_component_handle, &rawRotation))
		return {};

	return Vector3{ rawRotation.x, rawRotation.y, rawRotation.z };
}
void Transform::SetPosition(const Vector3& position) const
{
	const AE_API* api = ENGINEAPICONTEXT::Get();

	if (api)
		api->setTransformPosition(ae_component_handle, AE_VEC3{ position.x, position.y, position.z });
}

void Transform::SetRotation(const Vector3& rotation) const
{
	const AE_API* api = ENGINEAPICONTEXT::Get();
	
	if (api)
		api->setTransformRotation(ae_component_handle, AE_VEC3{ rotation.x, rotation.y, rotation.z });
}

void Transform::Rotate(const Vector3& rotate) const
{
	const AE_API* api = ENGINEAPICONTEXT::Get();

	if (api)
		api->setTransformRotate(ae_component_handle, AE_VEC3{ rotate.x, rotate.y, rotate.z });
}