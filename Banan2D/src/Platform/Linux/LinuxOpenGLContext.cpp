#include "bgepch.h"
#include "LinuxOpenGLContext.h"

#include "../../Banan/Core/ConsoleOutput.h"

#include "./LinuxWindow.h"

#include <glad/glad.h>

namespace Banan
{

	LinuxOpenGLContext::LinuxOpenGLContext(LinuxWindow* window) :
		m_window(window), m_glc(NULL)
	{
		BANAN_ASSERT(window, "Window handle is null");
	}

	LinuxOpenGLContext::~LinuxOpenGLContext()
	{
        // TODO NOT SURE YET
	}

	void LinuxOpenGLContext::Init()
	{
        GLXContext glc = glXCreateContext(m_window->GetDisplay(), m_window->GetVisualInfo(), NULL, GL_TRUE);
        glXMakeCurrent(m_window->GetDisplay(), m_window->GetWindow(), glc);

        gladLoadGL();

        BANAN_PRINT("OpenGL Info\n");
        BANAN_PRINT("  Vendor:       %s\n", glGetString(GL_VENDOR));
        BANAN_PRINT("  Renderer:     %s\n", glGetString(GL_RENDERER));
        BANAN_PRINT("  Version:      %s\n", glGetString(GL_VERSION));
        BANAN_PRINT("  GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	}

	void LinuxOpenGLContext::SwapBuffers()
	{
        glXSwapBuffers(m_window->GetDisplay(), m_window->GetWindow());
	}

	void LinuxOpenGLContext::SetVSync(bool enable)
	{
        glXSwapIntervalEXT(m_window->GetDisplay(), m_window->GetWindow(), enable);
	}

}