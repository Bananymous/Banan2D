#include "bgepch.h"
#include "OpenGLContext.h"

// TODO

#ifdef BANAN_PLATFORM_WINDOWS
	#include "../Windows/WindowsOpenGLContext.h"
	#include "../Windows/WindowsWindow.h"
#elif defined BANAN_PLATFORM_LINUX
	//#include "../Linux/LinuxOpenGLContext.h"
	//#include "../Linux/LinuxWindow.h"
#endif

namespace Banan
{

	Scope<RenderContext> OpenGLContext::Create(Window* window)
	{
#ifdef BANAN_PLATFORM_WINDOWS
		return CreateScope<WindowsOpenGLContext>(dynamic_cast<WindowsWindow*>(window));
#elif defined BANAN_PLATFORM_LINUX
		//return CreateScope<LinuxOpenGLContext>(dynamic_cast<WindowsWindow*>(window));
#else
	#error UNSUPPORTED PLATFORM
#endif

		return nullptr;
	}

}