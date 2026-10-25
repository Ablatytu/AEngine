#pragma once

#include "AComponent.h"
#include <cstdint>

class Camera final : public AComponent
{
public:
	enum class ProjectionCameraType : uint8_t
	{
		Perspective = 0,
		Orthographic = 1
	};
	Camera() = default;
	void SetProjection(ProjectionCameraType typeProj);
};