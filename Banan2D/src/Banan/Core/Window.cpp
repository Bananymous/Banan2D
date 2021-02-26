#include "bgepch.h"
#include "Window.h"

#ifdef BANAN_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsWindow.h"
#endif

namespace Banan
{

	Scope<Window> Window::Create(const std::wstring& title, uint32_t width, uint32_t height, bool vsync, const EventCallbackFn& callback)
	{
#ifdef BANAN_PLATFORM_WINDOWS
		return CreateScope<WindowsWindow>(title, width, height, vsync, callback);
#endif
	}

}