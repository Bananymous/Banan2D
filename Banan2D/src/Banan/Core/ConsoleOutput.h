#pragma once

#ifndef BANAN_DISTRIBUTION
	#ifdef BANAN_PLATFORM_WINDOWS
		#define BANAN_ENABLE_CONSOLE() FILE* stream; AllocConsole(); freopen_s(&stream, "CONOUT$", "wb", stdout)
		#define BANAN_DEBUG_BREAK() __debugbreak()
	#else
		#define BANAN_ENABLE_CONSOLE()
		#define BANAN_DEBUG_BREAK()
	#endif
	#define BANAN_PRINT(x) std::cout << x << std::endl
	#define BANAN_ASSERT(expr, msg) if(!(expr)) { std::cout << msg << std::endl; BANAN_DEBUG_BREAK(); } 
	#include <iostream>
#else
	#define BANAN_ENABLE_CONSOLE()
	#define BANAN_PRINT(x)
	#define BANAN_ASSERT(expr, msg)
#endif