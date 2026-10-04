#include "Vector2.h"
#include <cmath>

float Vector2::length() const
{
	return sqrt((x * x) + (y * y));
}
Vector2 Vector2::normalize() const
{
	float len = length();

	if(len == 0.0f)
		return Vector2(0.0f, 0.0f);

	return Vector2(x / len, y / len);
}

float Vector2::dot(const Vector2& other) const
{
	return x * other.x + y * other.y;
}