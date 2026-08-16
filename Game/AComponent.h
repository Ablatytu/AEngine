#pragma once

#include "../GameAPI/GameABI.h"
#include "ComponentSystem.h"
#include "ComponentStorage.h"
#include "Vector3.h"
#include <type_traits>
#define AE_REGISTER_CLASS(className) \
public: \
const char* GetComponentName() const override\
{\
	return #className; \
}\
private:\
struct RegisterClass \
{\
	RegisterClass();	\
};\
static RegisterClass registerClass_func;

#define AE_REGISTER_COMPONENT(className)\
className::RegisterClass className::registerClass_func{};\
className::RegisterClass::RegisterClass()\
{\
	ComponentSystem::Get().Register(#className, [](AE_GAMEOBJECT_HANDLE ae_gameobject_handle){ return ComponentStorage::Get().Add<className>(ae_gameobject_handle); });	\
}
#define AE_REGISTER_FIELD(valueName)\
RegisterFieldInInspector(#valueName, &valueName, GetTypeValue<decltype(valueName)>());
#define AE_SET_COLOR(valueName)\
SetColorFieldInInspector(#valueName);
#define AE_SET_RANGE(valueName, minValue, maxValue)\
SetRangeFieldInInspector(#valueName, minValue, maxValue);
 
class GameObject;
template<typename type> class AComponentRef;

class AComponent
{
public:
	std::vector<AE_GAME_FIELD_INFO::FieldInfo> fields;
	template<typename type> AE_GAME_FIELD_INFO::FieldType GetTypeValue()
	{
		if constexpr (std::is_enum_v<type>)
			return AE_GAME_FIELD_INFO::FieldType::ENUM;
		else if constexpr (std::is_same_v<type, int>)
			return AE_GAME_FIELD_INFO::FieldType::INT;
		else if constexpr (std::is_same_v<type, float>)
			return AE_GAME_FIELD_INFO::FieldType::FLOAT;
		else if constexpr (std::is_same_v<type, bool>)
			return AE_GAME_FIELD_INFO::FieldType::BOOL;
		else if constexpr (std::is_same_v<type, Vector3>)
			return AE_GAME_FIELD_INFO::FieldType::VEC3;
		else if constexpr (std::is_same_v<type, AE_VEC4>)
			return AE_GAME_FIELD_INFO::FieldType::VEC4;
		else
			static_assert(!std::is_same_v<type, type>, "UNKNOWN TYPE OF FIELD");
	}

	~AComponent();

	virtual void Start() {};
	
	virtual void Update(float deltaTime) {};

	virtual void OnInspector() {};

	virtual void OnDestroy() {};

	virtual const char* GetComponentName() const { return "New AComponent"; }
	GameObject GetGameObject() const;

	void RegisterFieldInInspector(const char* name, void* valueptr, AE_GAME_FIELD_INFO::FieldType type);
	void SetRangeFieldInInspector(const char* name, float minValue, float maxValue);
	void SetColorFieldInInspector(const char* name);

	explicit operator bool() const
	{
		return !AE_IsNull(ae_component_handle);
	}
	AE_COMPONENT_HANDLE GetHandle() const
	{
		return ae_component_handle;
	}
protected:
	AComponent() = default;
	friend class ComponentStorage;
	template<typename type> friend class AComponentRef;
	AE_COMPONENT_HANDLE ae_component_handle{};
	AE_GAMEOBJECT_HANDLE owner{};
};