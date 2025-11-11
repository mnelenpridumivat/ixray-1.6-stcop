#pragma once

#ifdef XRPHYSX_EXPORTS
#	define XRPHYSX_API __declspec(dllexport)
#else
#	define XRPHYSX_API __declspec(dllimport)
#endif