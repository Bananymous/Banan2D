#pragma once

#include "../../Banan/Core/Window.h"

#include <glad/glad_glx.h>

#include <X11/Xlib.h>

namespace Banan
{
	
	class RenderContext;

	class LinuxWindow : public Window
	{
	public:
		LinuxWindow(const std::wstring& title, uint32_t width, uint32_t height, bool vsync, const EventCallbackFn& callback);
		~LinuxWindow();

		virtual void OnUpdate(bool minimized) override;

		virtual uint32_t GetWidth() const override	{ return m_data.width; }
		virtual uint32_t GetHeight() const override { return m_data.height; }

		virtual void SetVSync(bool enable) override;
		virtual bool GetVSync() const override		{ return m_data.vsync; }

		virtual void SetTitle(const std::wstring& title) override;

		void* GetRenderContext();

	public:
		::Display* GetDisplay() { return m_data.display; }
		::XVisualInfo* GetVisualInfo() { return m_data.visualInfo; }
		::Window& GetWindow() { return m_data.window; }

	private:
		Scope<RenderContext> m_renderContext;

		struct WindowData
		{
			std::string title;
			uint32_t width;
			uint32_t height;
			bool vsync;
			EventCallbackFn eventCallback;

			::Display* display;
			::XVisualInfo* visualInfo;
			::Window window;
		};

		WindowData m_data;
	};

}
