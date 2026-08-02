#ifndef COMPONENT_H
#define COMPONENT_H
#include <vector>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include "../core/ComponentSystem.h"
#include "../core/AEngineAPI.h"
#include <type_traits>
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
#define SetColor(var) \
SetColorFieldInInspector(#var);
class GameObject;
class Material;
class Mesh;

enum class FieldType
{
	BOOL,
	FLOAT,
	VEC3,
	VEC4,
	INT,
	QUAT,
	MATERIAL,
	MESH,
	ENUM
};

struct FieldInfo
{
	const char* name;
	FieldType type;
	void* ptr;
	float minValue = 0.0f;
	float maxValue = 0.0f;
	bool isColor = false;
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
	template<typename t> inline FieldType fieldTypeInitial()
	{
		if constexpr (std::is_enum_v<t>)
			return FieldType::ENUM;
		else if constexpr (std::is_same_v<t, glm::quat>)
			return FieldType::QUAT;
		else if constexpr (std::is_same_v<t, float>)
			return FieldType::FLOAT;
		else if constexpr (std::is_same_v<t, bool>)
			return FieldType::BOOL;
		else if constexpr (std::is_same_v<t, int>)
			return FieldType::INT;
		else if constexpr (std::is_same_v<t, glm::vec3>)
			return FieldType::VEC3;
		else if constexpr (std::is_same_v<t, glm::vec4>)
			return FieldType::VEC4;
		else if constexpr (std::is_same_v<t, Material*>)
			return FieldType::MATERIAL;
		else if constexpr (std::is_same_v<t, Mesh*>)
			return FieldType::MESH;
		else
			static_assert(!std::is_same_v<t, t>, "UNKNOWN TYPE OF FIELD");
	}
	void RegisterFieldInInspector(const char* name, void* ptr, FieldType type);
	void SetRangeFieldInInspector(const char* name, float minValue, float maxValue);
	void SetColorFieldInInspector(const char* name);
};
#endif 
