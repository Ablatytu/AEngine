#pragma once
#include "../GameAPI/GameABI.h"

struct Vector4 : AE_VEC4
{
	Vector4() = default;

	Vector4(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}
};