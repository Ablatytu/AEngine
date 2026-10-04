#pragma once

#include "../GameAPI/GameABI.h"

#include "Transform.h"
#include "Camera.h"
#include "Light.h"
#include "Render.h"
#include "SpriteRenderer2D.h"
#include "ParticleSystem.h"

template<typename T>
struct EngineComponentWrapper
{
	static constexpr bool supported = false;
};

template<>
struct EngineComponentWrapper<Transform>
{
	static constexpr bool supported = true;

	static constexpr AE_COMPONENT_TYPE type = AE_COMPONENT_TYPE::AE_TRANSFORM;
};

template<> 
struct EngineComponentWrapper<SpriteRenderer2D>
{
	static constexpr bool supported = true;

	static constexpr AE_COMPONENT_TYPE type = AE_COMPONENT_TYPE::AE_SPRITERENDERER2D;
};

template<>
struct EngineComponentWrapper<ParticleSystem>
{
	static constexpr bool supported = true;

	static constexpr AE_COMPONENT_TYPE type = AE_COMPONENT_TYPE::AE_PARTICLESYSTEM;
};

template<>
struct EngineComponentWrapper<Camera>
{
	static constexpr bool supported = true;

	static constexpr AE_COMPONENT_TYPE type = AE_COMPONENT_TYPE::AE_CAMERA;
};

template<>
struct EngineComponentWrapper<Render>
{
	static constexpr bool supported = true;

	static constexpr AE_COMPONENT_TYPE type = AE_COMPONENT_TYPE::AE_RENDERER;
};

template<>
struct EngineComponentWrapper<Light>
{
	static constexpr bool supported = true;

	static constexpr AE_COMPONENT_TYPE type = AE_COMPONENT_TYPE::AE_LIGHT;
};