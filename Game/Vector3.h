#pragma once
#include "../GameAPI/GameABI.h"

struct Vector3 : AE_VEC3
{
	Vector3() = default;

	Vector3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
};