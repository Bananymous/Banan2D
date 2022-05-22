#pragma once

#include "Banan/Core/Window.h"

struct GLFWwindow;

namespace Banan
{

	class RenderContext;

	class GLFWWindow : public Window
	{
	public:
		GLFWWindow(const std::string& title, uint32_t width, uint32_t height, bool vsync, const EventCallbackFn& callback);
		~GLFWWindow();

		virtual void OnUpdate(bool minimized) override;

		virtual uint32_t GetWidth() const override { return m_data.width; }
		virtual uint32_t GetHeight() const override { return m_data.height; }

		virtual void SetVSync(bool enable) override;
		virtual bool GetVSync() const override { return m_data.vsync; }

		virtual void SetTitle(const std::string& title) override;

	public:
		GLFWwindow* GetWindowHandle() { return m_window; }

	private:
		void InitGLFWCallbacks();

	private:
		GLFWwindow* m_window;

		Scope<RenderContext> m_renderContext;

		struct WindowData
		{
			std::string title;
			uint32_t width;
			uint32_t height;
			bool vsync;
			EventCallbackFn eventCallback;
		};

		WindowData m_data;
	};

}