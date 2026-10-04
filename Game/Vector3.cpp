#include "Vector3.h"
#include <cmath>

float Vector3::length()
{
	return sqrt((x * x) + (y * y) + (z * z));
}
