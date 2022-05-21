#include "bgepch.h"

#ifdef BANAN_USE_GLFW

#include "GLFWWindow.h"

#include "../../Banan/Renderer/RendererAPI.h"
#include "../../Banan/Renderer/RenderContext.h"

#include "../../Banan/Event/WindowEvent.h"
#include "../../Banan/Event/KeyEvent.h"
#include "../../Banan/Event/MouseEvent.h"

#include <GLFW/glfw3.h>

namespace Banan
{

	static void GLFWErrorCallback(int error, const char* description)
	{
		BANAN_ERROR("GLFW Error (%d): %s\n", error, description);
	}

	GLFWWindow::GLFWWindow(const std::string& title, uint32_t width, uint32_t height, bool vsync, const EventCallbackFn& callback)
	{
		m_data.title = title;
		m_data.width = width;
		m_data.height = height;
		m_data.vsync = vsync;
		m_data.eventCallback = callback;

		int success = glfwInit();
		BANAN_ASSERT(success, "Could not initialize GLFW!\n");
		glfwSetErrorCallback(GLFWErrorCallback);
		
#ifdef BANAN_DEBUG
		if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL)
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

		m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

		m_renderContext = RenderContext::Create(this);
		m_renderContext->Init();

		glfwSetWindowUserPointer(m_window, &m_data);
		SetVSync(vsync);

		InitGLFWCallbacks();
	}

	GLFWWindow::~GLFWWindow()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	void GLFWWindow::OnUpdate(bool minimized)
	{
		glfwPollEvents();
		if (!minimized)
			m_renderContext->SwapBuffers();
	}

	void GLFWWindow::SetVSync(bool enabled)
	{
		m_data.vsync = enabled;
		m_renderContext->SetVSync(enabled);
	}

	void GLFWWindow::SetTitle(const std::string& title)
	{
		glfwSetWindowTitle(m_window, title.c_str());
	}

	void GLFWWindow::InitGLFWCallbacks()
	{
		glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.width = width;
				data.height = height;

				WindowResizeEvent event(width, height);
				data.eventCallback(event);
			});

		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.eventCallback(event);
			});

		glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressEvent event(key, 0);
					data.eventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleaseEvent event(key);
					data.eventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressEvent event(key, 1);
					data.eventCallback(event);
					break;
				}
				}
			});

		glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				KeyTypeEvent event(keycode);
				data.eventCallback(event);
			});

		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					MousePressEvent event(button);
					data.eventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseReleaseEvent event(button);
					data.eventCallback(event);
					break;
				}
				}
			});

		glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseScrollEvent event((float)yOffset);
				data.eventCallback(event);
			});

		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseMoveEvent event((int)xPos, (int)yPos);
				data.eventCallback(event);
			});
	}


}

#endif