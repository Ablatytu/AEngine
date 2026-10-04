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

namespace AE_GAME_RESOURCES_INFO
{
	struct AE_SHADER
	{
		const char* AE_FRAGMENT_SHADER_PATH = nullptr;
		const char* AE_VERTEX_SHADER_PATH = nullptr;
		const char* AE_GEOMETRY_SHADER_PATH = nullptr;
	};
	struct AE_MATERIAL
	{
		const char* name = nullptr;
	};
	struct AE_TEXTURE
	{
		const char* name = nullptr;
	};
}

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
	AE_RENDERER = 3,
	AE_SPRITERENDERER2D = 4,
	AE_PARTICLESYSTEM = 5
};

using AE_GET_TEXTURE_FN = bool (AE_CALL*)(const char* name, AE_GAME_RESOURCES_INFO::AE_TEXTURE* texture);
using AE_GET_SHADER_FN = bool(AE_CALL*)(const char* vertexPath, const char* fragmentPath, AE_GAME_RESOURCES_INFO::AE_SHADER* infoShader);
using AE_GET_MATERIAL_FN = bool(AE_CALL*)(const char* name, AE_GAME_RESOURCES_INFO::AE_MATERIAL* ae_material);

using AE_CREATE_GAMEOBJECT_FN = bool (AE_CALL*)(const char* name, AE_GAMEOBJECT_HANDLE* ae_gameobject_handle);
using AE_DESTROY_GAMEOBJECT_FN = void (AE_CALL*)(AE_GAMEOBJECT_HANDLE ae_gameobject_handle);

using AE_GETCOMPONENT_FN = bool (AE_CALL*)(AE_GAMEOBJECT_HANDLE ae_gameobject_handle, AE_COMPONENT_HANDLE* ae_component_handle, AE_COMPONENT_TYPE ae_component_type);
using AE_ADDCOMPONENT_FN = bool (AE_CALL*)(AE_GAMEOBJECT_HANDLE ae_gameobject_handle, AE_COMPONENT_HANDLE* ae_component_handle, AE_COMPONENT_TYPE ae_component_type);
using AE_ISENGINECOMPONENT_FN = bool (AE_CALL*)(AE_COMPONENT_HANDLE ae_component_handle);
using AE_ISGAMEOBJECT_FN = bool (AE_CALL*)(AE_GAMEOBJECT_HANDLE ae_gameobject_handle);

struct AE_VEC2
{
	float x = 0.0f;
	float y = 0.0f;
};
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

using AE_GET_KEY_FN = bool (AE_CALL*)(int32_t key);

using AE_SET_SPRITE_TEXTURE_FN = bool (AE_CALL*)(const char* name, AE_COMPONENT_HANDLE ae_component_handle);
using AE_SET_SPRITE_COLOR_FN = bool (AE_CALL*)(AE_VEC4 color, AE_COMPONENT_HANDLE ae_component_handle);

using AE_GET_PARTICLE_SYSTEM_MATERIAL_FN = bool(AE_CALL*)(AE_COMPONENT_HANDLE ae_component_handle, AE_GAME_RESOURCES_INFO::AE_MATERIAL* ae_material);
using AE_SET_PARTICLE_SYSTEM_MATERIAL_FN = bool(AE_CALL*)(AE_COMPONENT_HANDLE ae_component_handle, AE_GAME_RESOURCES_INFO::AE_MATERIAL ae_material);

using AE_SET_MATERIAL_TEXTURE = bool (AE_CALL*)(AE_GAME_RESOURCES_INFO::AE_MATERIAL ae_material, const char* nameField, AE_GAME_RESOURCES_INFO::AE_TEXTURE ae_texture);

using AE_GET_TRANSFORM_POSITION_FN = bool (AE_CALL*)(AE_COMPONENT_HANDLE ae_transform_component, AE_VEC3* ae_vec3);
using AE_GET_TRANSFORM_ROTATION_FN = bool (AE_CALL*)(AE_COMPONENT_HANDLE ae_transform_component, AE_VEC3* ae_vec3);
using AE_GET_TRANSFORM_SCALE_FN = bool (AE_CALL*)(AE_COMPONENT_HANDLE ae_transform_component, AE_VEC3* ae_vec3);
using AE_SET_TRANSFORM_POSITION_FN = bool (AE_CALL*)(AE_COMPONENT_HANDLE ae_transform_component, AE_VEC3 ae_vec3);
using AE_SET_TRANSFORM_ROTATION_FN = bool (AE_CALL*)(AE_COMPONENT_HANDLE ae_transform_component, AE_VEC3 ae_vec3);
using AE_SET_TRANSFORM_SCALE_FN = bool (AE_CALL*)(AE_COMPONENT_HANDLE ae_transform_component, AE_VEC3 ae_vec3);

using AE_SET_TRANSFORM_ROTATE_FN = bool (AE_CALL*)(AE_COMPONENT_HANDLE ae_tansform_component, AE_VEC3 ae_vec3);

using AE_LOG_FN = void (AE_CALL*)(const char* logMessage);

struct AE_API
{
	AE_LOG_FN log = NULL;

	AE_GET_KEY_FN getKey = NULL;

	AE_GETCOMPONENT_FN getComponent = NULL;
	AE_ISENGINECOMPONENT_FN isEngineComponent = NULL;
	AE_ISGAMEOBJECT_FN isGameObject = NULL;
	AE_ADDCOMPONENT_FN addComponent = NULL;

	AE_SET_SPRITE_TEXTURE_FN setSpriteTexture = NULL;
	AE_SET_SPRITE_COLOR_FN setSpriteColor = NULL;
	AE_GET_PARTICLE_SYSTEM_MATERIAL_FN getParticleSystemMaterial = NULL;
	AE_SET_PARTICLE_SYSTEM_MATERIAL_FN setParticleSystemMaterial = NULL;

	AE_SET_MATERIAL_TEXTURE setMaterialTexture = NULL;

	AE_GET_TRANSFORM_POSITION_FN getTransformPosition = NULL;
	AE_GET_TRANSFORM_ROTATION_FN getTransformRotation = NULL;
	AE_GET_TRANSFORM_SCALE_FN getTransformScale = NULL;

	AE_SET_TRANSFORM_POSITION_FN setTransformPosition = NULL;
	AE_SET_TRANSFORM_ROTATION_FN setTransformRotation = NULL;
	AE_SET_TRANSFORM_SCALE_FN setTransformScale = NULL;
	AE_SET_TRANSFORM_ROTATE_FN setTransformRotate = NULL;

	AE_CREATE_GAMEOBJECT_FN createGameObject = NULL;
	AE_DESTROY_GAMEOBJECT_FN destroyGameObject = NULL;

	AE_GET_TEXTURE_FN getTexture = NULL;
	AE_GET_SHADER_FN getShader = NULL;
	AE_GET_MATERIAL_FN getMaterial = NULL;
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
using AE_GAME_REMOVE_ACOMPONENTS_FN = void(AE_CALL*)(AE_GAMEOBJECT_HANDLE ae_gameobject_handle);

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
	AE_GAME_REMOVE_ACOMPONENTS_FN removeAComponentsFromGameObject = NULL;
	AE_GAME_ADD_ACOMPONENT_FN addAComponent = NULL;
};



using AE_GAME_API_FN = bool (AE_CALL*)(const AE_API* AEngineAPI, AE_GAMEAPI* GameAPI);