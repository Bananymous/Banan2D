#pragma once

#ifndef BGE_DISTRIBUTION
	#ifdef BGE_PLATFORM_WINDOWS
		#define BGE_ENABLE_CONSOLE() FILE* stream; AllocConsole(); freopen_s(&stream, "CONOUT$", "wb", stdout)
		#define BGE_DEBUG_BREAK() __debugbreak()
	#else
		#define BGE_ENABLE_CONSOLE()
		#define BGE_DEBUG_BREAK()
	#endif
	#define BGE_PRINT(x) std::cout << x << std::endl
	#define BGE_ASSERT(expr, msg) if(!(expr)) { std::cout << msg << std::endl; BGE_DEBUG_BREAK(); } 
	#include <iostream>
#else
	#define BGE_ENABLE_CONSOLE()
	#define BGE_PRINT(x)
	#define BGE_ASSERT(expr, msg)
#endif