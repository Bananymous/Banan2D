#pragma once

#include "Banan/Core/Window.h"

#include <Windows.h>

#pragma warning(push)
#pragma warning(disable: 26495)

namespace Banan
{
	
	class RenderContext;

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const std::string& title, uint32_t width, uint32_t height, bool vsync, const EventCallbackFn& callback);
		~WindowsWindow();

		virtual void OnUpdate(bool minimized) override;

		virtual uint32_t GetWidth() const override	{ return m_data.width; }
		virtual uint32_t GetHeight() const override { return m_data.height; }

		virtual void SetVSync(bool enable) override;
		virtual bool GetVSync() const override		{ return m_data.vsync; }

		virtual void SetTitle(const std::string& title) override;

	public:
		static void SetHInstance(HINSTANCE hInstance);

		bool IsFocused() const;

		HWND GetHandle() const			{ return m_data.hWnd; }
		HDC GetDeviceContext() const	{ return m_data.hDC; }

		void Init();

	private:
		// static WinProc Redirects WinProc messages to m_WinProc
		static LRESULT CALLBACK WinProc(HWND hWnd, UINT32 msg, WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK m_WinProc(HWND hWnd, UINT32 msg, WPARAM wParam, LPARAM lParam);

	private:
		static HINSTANCE s_hInstance;

		Scope<RenderContext> m_renderContext;

		struct WindowData
		{
			std::string title;
			uint32_t width;
			uint32_t height;
			bool vsync;
			EventCallbackFn eventCallback;

			HWND hWnd;
			HDC hDC;
		};

		bool m_initialized = false;

		WindowData m_data;
	};

}

#pragma warning(pop)