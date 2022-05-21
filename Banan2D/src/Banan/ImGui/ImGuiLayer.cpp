#include "bgepch.h"
#include "ImGuiLayer.h"

// TODO

#if BANAN_USE_GLFW
	#include "../../Platform/GLFW/GLFWImGuiLayer.h"
#elif defined BANAN_PLATFORM_WINDOWS
	#include "../../Platform/Windows/WindowsImGuiLayer.h"
#elif defined BANAN_PLATFORM_LINUX
	//#include "../../Platform/Linux/LinuxImGuiLayer.h"
#endif

namespace Banan
{

	ImGuiLayer* ImGuiLayer::Create()
	{
#if BANAN_USE_GLFW
		return new GLFWImGuiLayer();
#elif defined BANAN_PLATFORM_WINDOWS
		return new WindowsImGuiLayer();
#elif defined BANAN_PLATFORM_LINUX
		//return new LinuxImGuiLayer();
#else
	#error UNSUPPORTED PLATFORM
#endif

		return nullptr;
	}

}