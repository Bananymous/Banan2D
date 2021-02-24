#include "bgepch.h"
#include "OpenGLContext.h"

#ifdef BGE_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsOpenGLContext.h"
	#include "Platform/Windows/WindowsWindow.h"
#endif

namespace Banan
{

	Scope<RenderContext> OpenGLContext::Create(Window* window)
	{
#ifdef BGE_PLATFORM_WINDOWS
		return CreateScope<WindowsOpenGLContext>(dynamic_cast<WindowsWindow*>(window));
#endif
	}

}