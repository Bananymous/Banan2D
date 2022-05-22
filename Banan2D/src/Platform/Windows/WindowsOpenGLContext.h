#pragma once

#include "Platform/OpenGL/OpenGLContext.h"

#include <Windows.h>

namespace Banan
{
	class WindowsWindow;

	class WindowsOpenGLContext : public OpenGLContext
	{
	public:
		WindowsOpenGLContext(WindowsWindow* window);
		~WindowsOpenGLContext();

		virtual void Init() override;
		virtual void SwapBuffers() override;

		void SetVSync(bool enable);

	private:
		HGLRC m_hGLRC;
		WindowsWindow* m_window;
	};

}