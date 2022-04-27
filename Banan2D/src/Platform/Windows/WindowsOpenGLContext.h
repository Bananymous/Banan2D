#pragma once

#include "../OpenGL/OpenGLContext.h"

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

		virtual void* GetContext() override { return m_hGLRC; };

		void SetVSync(bool enable);

	private:
		HGLRC m_hGLRC;
		WindowsWindow* m_window;
	};

}