#pragma once
#ifdef _WIN32
	#ifdef AENGINE_BUILD_DLL
		#define AENGINE_API __declspec(dllexport)
	#else
		#define AENGINE_API __declspec(dllimport)
	#endif
#else
	#define AENGINE_API
#endif