#include "EngineAPIContext.h"

namespace
{
	AE_API ae_api{};
	bool isClear = true;
}

namespace ENGINEAPICONTEXT
{
	bool Bind(const AE_API* api)
	{
		if (!api)
			return false;

		if (!api->log)
			return false;

		ae_api = *api;
		isClear = false;

		return true;
	}

	const AE_API* Get()
	{
		if (isClear)
			return nullptr;
		return &ae_api;
	}

	void UnBind()
	{
		ae_api = {};
		isClear = true;
	}
}