#include "bgepch.h"
#include "Window.h"

#ifdef BANAN_USE_GLFW
	#include "Platform/GLFW/GLFWWindow.h"
#elif defined BANAN_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsWindow.h"
#elif defined BANAN_PLATFORM_LINUX
	#include "Platform/Linux/LinuxWindow.h"
#endif

namespace Banan
{

	Scope<Window> Window::Create(const std::string& title, uint32_t width, uint32_t height, bool vsync, const EventCallbackFn& callback)
	{
#if BANAN_USE_GLFW
		return CreateScope<GLFWWindow>(title, width, height, vsync, callback);
#elif defined BANAN_PLATFORM_WINDOWS
		return CreateScope<WindowsWindow>(title, width, height, vsync, callback);
#elif defined BANAN_PLATFORM_LINUX
		return CreateScope<LinuxWindow>(title, width, height, vsync, callback);
#endif
		return nullptr;
	}

}