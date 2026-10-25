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

	Vector3 operator*(const float value) const;
	Vector3 operator+(const Vector3& vec3) const;

	void Normalize();
	float length();
};