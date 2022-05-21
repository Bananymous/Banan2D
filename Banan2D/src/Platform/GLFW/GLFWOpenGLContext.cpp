#include "bgepch.h"

#if BANAN_USE_GLFW

#include "GLFWOpenGLContext.h"

#include "./GLFWWindow.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Banan
{

	GLFWOpenGLContext::GLFWOpenGLContext(GLFWWindow* window) :
		m_window(window)
	{ }

	void GLFWOpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_window->GetWindowHandle());
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		BANAN_ASSERT(status, "Failed to initialize Glad!\n");

		BANAN_PRINT("OpenGL Info\n");
		BANAN_PRINT("  Vendor:       %s\n", glGetString(GL_VENDOR));
		BANAN_PRINT("  Renderer:     %s\n", glGetString(GL_RENDERER));
		BANAN_PRINT("  Version:      %s\n", glGetString(GL_VERSION));
		BANAN_PRINT("  GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	}

	void GLFWOpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_window->GetWindowHandle());
	}

	void GLFWOpenGLContext::SetVSync(bool enabled)
	{
		glfwSwapInterval((int)enabled);
	}

}

#endif