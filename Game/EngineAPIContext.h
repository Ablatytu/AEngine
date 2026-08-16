#pragma once

#include "../GameAPI/GameABI.h"

namespace ENGINEAPICONTEXT
{
	bool Bind(const AE_API* api);

	const AE_API* Get();
	
	void UnBind();
}