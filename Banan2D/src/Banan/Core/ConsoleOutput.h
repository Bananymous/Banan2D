#pragma once

#include "Banan/Core/Base.h"

#ifndef BANAN_DISABLE_CONSOLE
	#define PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__
	#define CAT(a, ...) PRIMITIVE_CAT(a, __VA_ARGS__)

	#define CHECK_N(x, n, ...) n
	#define CHECK(...) CHECK_N(__VA_ARGS__, 0,)
	#define PROBE(x) x, 1,

	#define IS_1(x) CHECK(PRIMITIVE_CAT(IS_1_, x))
	#define IS_1_1 PROBE(~)

	#define NARGS_SEQ(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, N, ...) N
	#define NARGS(...) NARGS_SEQ(__VA_ARGS__, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

	#define BANAN_PRINT_1(str) printf("%s", str)
	#define BANAN_PRINT_0(...) printf(__VA_ARGS__)

	#define BANAN_PRINT(...) CAT(BANAN_PRINT_, IS_1(NARGS(__VA_ARGS__)))(__VA_ARGS__) 

	#define BANAN_WARN(...)  printf("\x1b[93m"); BANAN_PRINT(__VA_ARGS__); printf("\x1b[0m")
	#define BANAN_ERROR(...) printf("\x1b[91m"); BANAN_PRINT(__VA_ARGS__); printf("\x1b[0m")

	#define BANAN_ASSERT(expr, ...) if(!(expr)) { BANAN_ERROR(__VA_ARGS__); BANAN_DEBUG_BREAK(); } 
	#include <cstdio>
#else
	#define BANAN_PRINT(...)
	#define BANAN_WARN(...)
	#define BANAN_ERROR(...)
	#define BANAN_ASSERT(expr, ...)
#endif