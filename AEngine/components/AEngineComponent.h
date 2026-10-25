#ifndef COMPONENT_H
#define COMPONENT_H
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../core/ComponentSystem.h"
#include "../core/AEngineAPI.h"
#include "../components/ComponentHandle.h"
#include <type_traits>
#define AE_REGISTER_CLASS(className) \
public: \
const char* GetComponentName() const override \
{ \
	return #className; \
}\
private:\
struct RegisterClassFunc\
{\
  RegisterClassFunc();  \
};\
static RegisterClassFunc registerClassFunc;
#define AE_REGISTER_COMPONENT(className) \
className::RegisterClassFunc className::registerClassFunc{}; \
className::RegisterClassFunc::RegisterClassFunc() \
{\
	ComponentSystem::Register(#className, [](GameObject* object){return object->AddComponent<className>();}); \
}
#define Range(var, minValue, maxValue) \
SetRangeFieldInInspector(#var, minValue, maxValue);
#define RegisterField(var) \
RegisterFieldInInspector(#var, &var, fieldTypeInitial<decltype(var)>());
#define SetColor(var) \
SetColorFieldInInspector(#var);
#define RegisterEnumNames(var, ...) \
RegisterEnumNamesInInspector(#var, __VA_ARGS__);

class GameObject;
class Material;
class Mesh;
class Texture;
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
	ENUM,
	TEXTURE2D
};

struct FieldInfo
{
	const char* name;
	FieldType type;
	void* ptr;
	float minValue = 0.0f;
	float maxValue = 0.0f;
	bool isColor = false;

	std::vector<const char*> enumNames;
};
class AENGINE_API AEngineComponent
{
public:
	GameObject* gameObject;
	ComponentHandle handle;
	std::vector<FieldInfo> fields;
	static constexpr bool AlwaysWorksComponent = false;
	AEngineComponent() = default;
	virtual ~AEngineComponent() {};
	virtual void Update() {}
	virtual void Start() {}
	virtual void Awake() {}
	virtual void OnInspector() {}
	virtual void OnDestroy() {}
	virtual const char* GetComponentName() const { return "New AEngineComponent"; }
	template<typename t> inline FieldType fieldTypeInitial()
	{
		if constexpr (std::is_enum_v<t>)
		{
			static_assert(std::is_same_v<std::underlying_type_t<t>, int>, "NOT SUPPORTED TYPE IN ENUM\n");
			return FieldType::ENUM;
		}
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
		else if constexpr (std::is_same_v<t, Texture*>)
			return FieldType::TEXTURE2D;
		else
			static_assert(!std::is_same_v<t, t>, "UNKNOWN TYPE OF FIELD");
	}
	void RegisterFieldInInspector(const char* name, void* ptr, FieldType type);
	void SetRangeFieldInInspector(const char* name, float minValue, float maxValue);
	void SetColorFieldInInspector(const char* name);
	template<typename... names> void RegisterEnumNamesInInspector(const char* name, names... enumNames)
	{
		for (FieldInfo& info : fields)
		{
			if (strcmp(info.name, name) == 0 && info.type == FieldType::ENUM)
			{
				info.enumNames = { enumNames... };
				break;
			}
		}
	}
};
#endif 
