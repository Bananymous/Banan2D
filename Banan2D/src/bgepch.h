#pragma once

#include "Banan/Core/PlatformDetection.h"

#ifdef BANAN_PLATFORM_WINDOWS
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
#endif

#include <iostream>
#include <memory>
#include <algorithm>
#include <functional>
#include <utility>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Banan/Core/ConsoleOutput.h"

#ifdef BANAN_PLATFORM_WINDOWS
	#include <Windows.h>
	#include <Windowsx.h>
#endif