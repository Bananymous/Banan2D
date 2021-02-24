#pragma once

#include "BGE/Core/PlatformDetection.h"

#ifdef BGE_PLATFORM_WINDOWS
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

#include "BGE/Core/ConsoleOutput.h"

#ifdef BGE_PLATFORM_WINDOWS
	#include <Windows.h>
	#include <Windowsx.h>
#endif