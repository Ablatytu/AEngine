#include "AEngineComponent.h"


void AEngineComponent::RegisterFieldInInspector(const char* name, void* ptr, FieldType type)
{
	fields.push_back({ name, type, ptr });
}
void AEngineComponent::SetRangeFieldInInspector(const char* name, float minValue, float maxValue)
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
void AEngineComponent::SetColorFieldInInspector(const char* name)
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
