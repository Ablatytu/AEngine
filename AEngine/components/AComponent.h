#ifndef COMPONENT_H
#define COMPONENT_H
#include <vector>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include "../core/ComponentSystem.h"
#include "../core/AEngineAPI.h"

#define RegisterComponent(className) \
public:                                                              \
const char* GetComponentName() const override \
{ \
	return #className; \
}
#define Range(var, minValue, maxValue) \
SetRangeFieldInInspector(#var, minValue, maxValue);
#define RegisterField(var) \
RegisterFieldInInspector(#var, &var, fieldTypeInitial<decltype(var)>());
class GameObject;


enum class FieldType
{
	BOOL,
	FLOAT,
	VEC3,
	INT,
	QUAT
};

struct FieldInfo
{
	const char* name;
	FieldType type;
	void* ptr;
	float minValue = 0.0f;
	float maxValue = 0.0f;
};
class AENGINE_API AComponent
{
public:
	GameObject* gameObject;
	std::vector<FieldInfo> fields;
	AComponent() = default;
	virtual ~AComponent() {};
	virtual void Update() {}
	virtual void Start() {}
	virtual void Awake() {}
	virtual void OnInspector() {}
	virtual void OnDestroy() {}
	virtual const char* GetComponentName() const { return "New AComponent"; }
	template<typename t> FieldType fieldTypeInitial();
	template<> inline FieldType fieldTypeInitial<glm::quat>()
	{
		return FieldType::QUAT;
	}
	template<> inline FieldType fieldTypeInitial<float>()
	{
		return FieldType::FLOAT;
	}
	template<> inline FieldType fieldTypeInitial<bool>()
	{
		return FieldType::BOOL;
	}
	template<> inline FieldType fieldTypeInitial<int>()
	{
		return FieldType::INT;
	}
	template<> inline FieldType fieldTypeInitial<glm::vec3>()
	{
		return FieldType::VEC3;
	}
	void RegisterFieldInInspector(const char* name, void* ptr, FieldType type);
	void SetRangeFieldInInspector(const char* name, float minValue, float maxValue);
};
#endif 
