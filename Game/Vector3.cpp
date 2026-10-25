#include "Vector3.h"
#include <cmath>

float Vector3::length()
{
	return sqrt((x * x) + (y * y) + (z * z));
}

void Vector3::Normalize()
{
	float lengthVector = length();

	if (lengthVector < 0.0001f)
		return;

	x /= lengthVector;
	y /= lengthVector;
	z /= lengthVector;
}

Vector3 Vector3::operator*(const float value) const
{
	return Vector3(x * value, y * value, z * value);
}
Vector3 Vector3::operator+(const Vector3& vec3) const
{
	return Vector3(x + vec3.x, y + vec3.y, z + vec3.z);
}