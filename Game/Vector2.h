#pragma once
#include "../GameAPI/GameABI.h"

struct Vector2 : AE_VEC2
{
	Vector2() = default;

	Vector2(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	float length() const;

	Vector2 normalize() const;

	float dot(const Vector2& other) const;
};