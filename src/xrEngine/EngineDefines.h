#pragma once

#ifdef ENGINE_BUILD
#define DLL_API			__declspec(dllimport)
#define ENGINE_API		__declspec(dllexport)
#else
#undef	DLL_API
#define DLL_API			__declspec(dllexport)
#define ENGINE_API		__declspec(dllimport)
#endif

#ifndef ECORE_API
#	define ECORE_API
#endif