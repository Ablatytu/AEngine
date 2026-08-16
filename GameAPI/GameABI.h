#pragma once

#include <cstdint>

#ifdef _WIN32
	#define AE_CALL __cdecl
	#define AE_EXPORT extern "C" __declspec(dllexport)
#else
	#define AE_CALL 
	#define AE_EXPORT extern "C" 
#endif


struct AE_GAMEOBJECT_HANDLE
{
	uint32_t index = UINT32_MAX;
	uint32_t generation = 0;
};

struct AE_COMPONENT_HANDLE
{
	uint32_t index = UINT32_MAX;
	uint32_t generation = 0;
	
};
namespace AE_GAME_FIELD_INFO
{
	enum FieldType : uint32_t
	{
		BOOL,
		FLOAT,
		VEC3,
		VEC4,
		INT,
		ENUM
	};
	struct FieldInfo
	{
		const char* nameField;
		FieldType typeValue;
		void* ptr;
		float minValue = 0.0f;
		float maxValue = 0.0f;
		bool isColor = false;
	};
}
struct AE_COMPONENT_INFO
{
	AE_GAMEOBJECT_HANDLE ae_gameObject_handle{};
	AE_COMPONENT_HANDLE ae_component_handle{};
	const char* name = ""; 
	AE_GAME_FIELD_INFO::FieldInfo* fieldsInfo;
	int countFieldsInfo = 0;
};

inline bool AE_IsNull(AE_GAMEOBJECT_HANDLE ae_gameobject_handle)
{
	return ae_gameobject_handle.index == UINT32_MAX;
}

inline bool AE_IsNull(AE_COMPONENT_HANDLE ae_component_handle)
{
	return ae_component_handle.index == UINT32_MAX;
}

enum AE_COMPONENT_TYPE : uint32_t
{
	AE_TRANSFORM = 0,
	AE_CAMERA = 1,
	AE_LIGHT = 2,
	AE_RENDERER = 3
};


using AE_GETCOMPONENT_FN = bool (AE_CALL*)(AE_GAMEOBJECT_HANDLE ae_gameobject_handle, AE_COMPONENT_HANDLE* ae_component_handle, AE_COMPONENT_TYPE ae_component_type);
using AE_ADDCOMPONENT_FN = bool (AE_CALL*)(AE_GAMEOBJECT_HANDLE ae_gameobject_handle, AE_COMPONENT_HANDLE* ae_component_handle, AE_COMPONENT_TYPE ae_component_type);
using AE_ISENGINECOMPONENT_FN = bool (AE_CALL*)(AE_COMPONENT_HANDLE ae_component_handle);


struct AE_VEC3
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};
struct AE_VEC4
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 0.0f;
};

using AE_GET_TRANSFORM_POSITION_FN = bool (AE_CALL*)(AE_COMPONENT_HANDLE ae_transform_component, AE_VEC3* ae_vec3);
using AE_GET_TRANSFORM_ROTATION_FN = bool (AE_CALL*)(AE_COMPONENT_HANDLE ae_transform_component, AE_VEC3* ae_vec3);
using AE_SET_TRANSFORM_POSITION_FN = bool (AE_CALL*)(AE_COMPONENT_HANDLE ae_transform_component, AE_VEC3 ae_vec3);
using AE_SET_TRANSFORM_ROTATION_FN = bool (AE_CALL*)(AE_COMPONENT_HANDLE ae_transform_component, AE_VEC3 ae_vec3);
using AE_SET_TRANSFORM_ROTATE_FN = bool (AE_CALL*)(AE_COMPONENT_HANDLE ae_tansform_component, AE_VEC3 ae_vec3);

using AE_LOG_FN = void (AE_CALL*)(const char* logMessage);

struct AE_API
{
	AE_LOG_FN log = NULL;
	AE_GETCOMPONENT_FN getComponent = NULL;

	AE_GET_TRANSFORM_POSITION_FN getTransformPosition = NULL;
	AE_GET_TRANSFORM_ROTATION_FN getTransformRotation = NULL;
	AE_SET_TRANSFORM_POSITION_FN setTransformPosition = NULL;
	AE_SET_TRANSFORM_ROTATION_FN setTransformRotation = NULL;
	AE_SET_TRANSFORM_ROTATE_FN setTransformRotate = NULL;
	AE_ISENGINECOMPONENT_FN isEngineComponent = NULL;
	AE_ADDCOMPONENT_FN addComponent = NULL;
};



using AE_GAME_SHUTDOWN_FN = void(AE_CALL*)();
using AE_GAME_UPDATE_FN = void(AE_CALL*)(float deltaTime);
using AE_GAME_START_FN = void(AE_CALL*)();

using AE_GAME_GET_REGISTERED_ACOMPONENT_NAME_FN = const char*(AE_CALL*)(unsigned int ID);
using AE_GAME_GET_REGISTERED_ACOMPONENTS_COUNT_FN = const int(AE_CALL*)();
using AE_GAME_GET_ACOMPONENT_INFO_FN = void(AE_CALL*)(AE_COMPONENT_INFO* ae_component_info_storage, int size);
using AE_GAME_GET_ACOMPONENT_COUNT_FN = int(AE_CALL*)();
using AE_GAME_CLEAR_ACOMPONENTS = void(AE_CALL*)();
//using AE_GAME_SET_TEST_TRANSFORM_POSITION_OBJECT = void (AE_CALL*)(AE_GAMEOBJECT_HANDLE ae_object_handle);
using AE_GAME_ADD_ACOMPONENT_FN = void(AE_CALL*)(unsigned int ID, AE_GAMEOBJECT_HANDLE ae_gameObject_handle);
using AE_GAME_GET_ACOMPONENTS_COUNT_FROM_GAMEOBJECT = int(AE_CALL*)(AE_GAMEOBJECT_HANDLE ae_gameObject_handle);
using AE_GAME_GET_ACOMPONENTS_INFO_FROM_GAMEOBJECT = void(AE_CALL*)(AE_GAMEOBJECT_HANDLE ae_gameObject_handle, AE_COMPONENT_INFO* ae_component_handle, int size);
using AE_GAME_REMOVE_ACOMPONENT_FN = void(AE_CALL*)(AE_GAMEOBJECT_HANDLE ae_gameobject_handle, AE_COMPONENT_HANDLE ae_component_handle);

struct AE_GAMEAPI
{
	AE_GAME_SHUTDOWN_FN shutdown = NULL;
	AE_GAME_UPDATE_FN update = NULL;
	AE_GAME_START_FN start = NULL;

	AE_GAME_CLEAR_ACOMPONENTS clearGameAComponents = NULL;
	AE_GAME_GET_REGISTERED_ACOMPONENTS_COUNT_FN getRegisteredAComponentsCount = NULL;
	AE_GAME_GET_REGISTERED_ACOMPONENT_NAME_FN getRegisteredAComponentName = NULL;
	AE_GAME_GET_ACOMPONENTS_INFO_FROM_GAMEOBJECT getAComponentsFromGameObject = NULL;
	AE_GAME_GET_ACOMPONENTS_COUNT_FROM_GAMEOBJECT getAComponentsCountFromGameObject = NULL;
	AE_GAME_GET_ACOMPONENT_COUNT_FN getAComponentCount = NULL;
	AE_GAME_GET_ACOMPONENT_INFO_FN getAComponentsInfo = NULL;
	AE_GAME_REMOVE_ACOMPONENT_FN removeAComponent = NULL;
	AE_GAME_ADD_ACOMPONENT_FN addAComponent = NULL;
};



using AE_GAME_API_FN = bool (AE_CALL*)(const AE_API* AEngineAPI, AE_GAMEAPI* GameAPI);