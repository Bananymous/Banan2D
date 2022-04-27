#pragma once

#include "../OpenGL/OpenGLContext.h"

#include <glad/glad_glx.h>

namespace Banan
{
	class LinuxWindow;

	class LinuxOpenGLContext : public OpenGLContext
	{
	public:
		LinuxOpenGLContext(LinuxWindow* window);
		~LinuxOpenGLContext();

		virtual void Init() override;
		virtual void SwapBuffers() override;

		virtual void* GetContext() override { return m_glc; };

		void SetVSync(bool enable);

	private:
        GLXContext m_glc;
		LinuxWindow* m_window;
	};

}