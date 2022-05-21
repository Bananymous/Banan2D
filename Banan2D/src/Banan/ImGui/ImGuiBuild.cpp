#include "bgepch.h"

#ifndef BANAN_DISTRIBUTION

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <backends/imgui_impl_opengl3.cpp>

#if BANAN_USE_GLFW
	#include <backends/imgui_impl_glfw.cpp>
#elif defined BANAN_PLATFORM_WINDOWS
	#include <backends/imgui_impl_win32.cpp>
#elif defined BANAN_PLATFORM_LINUX
	//#include <backends/imgui_impl_glfw.cpp>
#endif

#endif