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
		BGE_ASSERT(window, "Window handle is null");
	}

	WindowsOpenGLContext::~WindowsOpenGLContext()
	{
		int status = wglDeleteContext(m_hGLRC);
		BGE_ASSERT(status, "Could not delete OpenGL context!");
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
		BGE_ASSERT(pixelformat, "Failed to choose fitting pixel format!");

		int status = SetPixelFormat(m_window->GetDeviceContext(), pixelformat, &pfd);
		BGE_ASSERT(status, "Could not set pixel format!");

		m_hGLRC = wglCreateContext(m_window->GetDeviceContext());
		BGE_ASSERT(m_hGLRC, "Could not create OpenGL context!");

		status = wglMakeCurrent(m_window->GetDeviceContext(), m_hGLRC);
		BGE_ASSERT(status, "Could not change current context!");

		status = gladLoadWGL(m_window->GetDeviceContext());
		BGE_ASSERT(status, "Could not load GladWLG!");

		status = gladLoadGL();
		BGE_ASSERT(status, "Could not load GladGL!");

		BGE_PRINT("OpenGL Info");
		BGE_PRINT("  Vendor:\t"			<< glGetString(GL_VENDOR));
		BGE_PRINT("  Renderer:\t"		<< glGetString(GL_RENDERER));
		BGE_PRINT("  Version:\t"		<< glGetString(GL_VERSION));
		BGE_PRINT("  GLSL Version:\t"	<< glGetString(GL_SHADING_LANGUAGE_VERSION));
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