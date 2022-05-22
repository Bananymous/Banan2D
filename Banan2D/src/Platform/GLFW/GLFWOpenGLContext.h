#pragma once

#include "Banan/Renderer/RenderContext.h"

namespace Banan
{
	class GLFWWindow;

	class GLFWOpenGLContext : public RenderContext
	{
	public:
		GLFWOpenGLContext(GLFWWindow* window);

		virtual void Init() override;
		virtual void SwapBuffers() override;

		virtual void SetVSync(bool enable) override;

	private:
		GLFWWindow* m_window;
	};
}