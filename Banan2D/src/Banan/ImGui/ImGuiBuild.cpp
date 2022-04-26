#include "bgepch.h"

#ifndef BANAN_DISTRIBUTION

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <backends/imgui_impl_opengl3.cpp>

#ifdef BANAN_PLATFORM_WINDOWS

#include <backends/imgui_impl_win32.cpp>

#elif defined BANAN_PLATFORM_LINUX

// LINUX IMPL (GLFW?)
// #include <backends/imgui_impl_glfw.cpp>

#endif

#endif