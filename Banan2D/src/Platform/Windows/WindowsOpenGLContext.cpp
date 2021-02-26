#include "bgepch.h"
#include "WindowsOpenGLContext.h"

#include "WindowsWindow.h"

#include <glad/glad.h>
#include <glad/glad_wgl.h>

namespace Banan
{

	WindowsOpenGLContext::WindowsOpenGLContext(WindowsWindow* window) :
		m_window(window), m_hGLRC(NULL)
	{
		BANAN_ASSERT(window, "Window handle is null");
	}

	WindowsOpenGLContext::~WindowsOpenGLContext()
	{
		int status = wglDeleteContext(m_hGLRC);
		BANAN_ASSERT(status, "Could not delete OpenGL context!");
	}

	void WindowsOpenGLContext::Init()
	{

		PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,
			24,
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			32,
			0,
			0,
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};

		int pixelformat = ChoosePixelFormat(m_window->GetDeviceContext(), &pfd);
		BANAN_ASSERT(pixelformat, "Failed to choose fitting pixel format!");

		int status = SetPixelFormat(m_window->GetDeviceContext(), pixelformat, &pfd);
		BANAN_ASSERT(status, "Could not set pixel format!");

		m_hGLRC = wglCreateContext(m_window->GetDeviceContext());
		BANAN_ASSERT(m_hGLRC, "Could not create OpenGL context!");

		status = wglMakeCurrent(m_window->GetDeviceContext(), m_hGLRC);
		BANAN_ASSERT(status, "Could not change current context!");

		status = gladLoadWGL(m_window->GetDeviceContext());
		BANAN_ASSERT(status, "Could not load GladWLG!");

		status = gladLoadGL();
		BANAN_ASSERT(status, "Could not load GladGL!");

		BANAN_PRINT("OpenGL Info");
		BANAN_PRINT("  Vendor:\t"			<< glGetString(GL_VENDOR));
		BANAN_PRINT("  Renderer:\t"		<< glGetString(GL_RENDERER));
		BANAN_PRINT("  Version:\t"		<< glGetString(GL_VERSION));
		BANAN_PRINT("  GLSL Version:\t"	<< glGetString(GL_SHADING_LANGUAGE_VERSION));
	}

	void WindowsOpenGLContext::SwapBuffers()
	{
		wglSwapLayerBuffers(m_window->GetDeviceContext(), WGL_SWAP_MAIN_PLANE);
	}

	void WindowsOpenGLContext::SetVSync(bool enable)
	{
		wglSwapIntervalEXT(enable);
	}

}