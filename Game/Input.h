#pragma once
#include "InputKeys.h"

class Input
{
public:
	static bool GetKey(KeyCode keycode);
	static float GetAxis(const char* nameAxis);
};