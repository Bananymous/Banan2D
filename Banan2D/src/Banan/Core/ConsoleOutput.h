#pragma once

#ifndef BANAN_DISABLE_CONSOLE
	#define BANAN_PRINT(...) std::printf(__VA_ARGS__)
	#define BANAN_WARN(...) std::printf("\x1b[93m"); std::printf(__VA_ARGS__); std::printf("\x1b[0m")
	#define BANAN_ERROR(...) std::printf("\x1b[91m"); std::printf(__VA_ARGS__); std::printf("\x1b[0m")
	#define BANAN_ASSERT(expr, ...) if(!(expr)) { std::printf(__VA_ARGS__); BANAN_DEBUG_BREAK(); } 
	#include <iostream>
#else
	#define BANAN_PRINT(...)
	#define BANAN_WARN(...)
	#define BANAN_ERROR(...)
	#define BANAN_ASSERT(expr, ...)
#endif