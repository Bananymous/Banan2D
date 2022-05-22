#include "bgepch.h"
#include "OpenGLContext.h"

#ifdef BANAN_USE_GLFW
	#include "../GLFW/GLFWOpenGLContext.h"
	#include "../GLFW/GLFWWindow.h"
#elif defined BANAN_PLATFORM_WINDOWS
	#include "../Windows/WindowsOpenGLContext.h"
	#include "../Windows/WindowsWindow.h"
#elif defined BANAN_PLATFORM_LINUX
	#include "../Linux/LinuxOpenGLContext.h"
	#include "../Linux/LinuxWindow.h"
#endif

namespace Banan
{

	Scope<RenderContext> OpenGLContext::Create(Window* window)
	{
#ifdef BANAN_USE_GLFW
		return CreateScope<GLFWOpenGLContext>(dynamic_cast<GLFWWindow*>(window));
#elif defined BANAN_PLATFORM_WINDOWS
		return CreateScope<WindowsOpenGLContext>(dynamic_cast<WindowsWindow*>(window));
#elif defined BANAN_PLATFORM_LINUX
		return CreateScope<LinuxOpenGLContext>(dynamic_cast<LinuxWindow*>(window));
#endif
		return nullptr;
	}

}