#include "AComponent.h"


void AComponent::RegisterFieldInInspector(const char* name, void* ptr, FieldType type)
{
	fields.push_back({ name, type, ptr });
}
void AComponent::SetRangeFieldInInspector(const char* name, float minValue, float maxValue)
{
	for (FieldInfo& field : fields)
	{
		if (strcmp(field.name, name) == 0)
		{
			field.minValue = minValue;
			field.maxValue = maxValue;
			return;
		}
	}
}
void AComponent::SetColorFieldInInspector(const char* name)
{
	for (FieldInfo& field : fields)
	{
		if (strcmp(field.name, name) == 0)
		{
			if (field.type == FieldType::VEC4 || field.type == FieldType::VEC3)
			{
				field.isColor = true;
				return;
			}
		}
	}
}