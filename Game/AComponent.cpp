#include "AComponent.h"
#include "GameObject.h"
GameObject AComponent::GetGameObject() const
{
	return GameObject(owner);
}

AComponent::~AComponent()
{
	fields.clear();
}
void AComponent::RegisterFieldInInspector(const char* name, void* valueptr, AE_GAME_FIELD_INFO::FieldType type)
{
	if (type == AE_GAME_FIELD_INFO::FieldType::VEC3)
	{
		Vector3* vec3 = static_cast<Vector3*>(valueptr);
		AE_VEC3* ae_vec3 = static_cast<AE_VEC3*>(vec3);

		fields.push_back({ name, type, ae_vec3 });
		return;
	}
	fields.push_back({ name, type, valueptr });
}
void AComponent::SetRangeFieldInInspector(const char* name, float minValue, float maxValue)
{
	for (AE_GAME_FIELD_INFO::FieldInfo& field : fields)
	{
		if (strcmp(name, field.nameField) == 0)
		{
			field.minValue = minValue;
			field.maxValue = maxValue;
			break;
		}
	}
}
void AComponent::SetColorFieldInInspector(const char* name)
{
	for (AE_GAME_FIELD_INFO::FieldInfo& field : fields)
	{
		if (strcmp(name, field.nameField) == 0)
		{
			field.isColor = true;
			break;
		}
	}
}