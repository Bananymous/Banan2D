#include "bgepch.h"
#include "WindowsWindow.h"

#include "Banan/Event/WindowEvent.h"
#include "Banan/Event/MouseEvent.h"
#include "Banan/Event/KeyEvent.h"

#include "Banan/Renderer/RenderContext.h"

#include "Platform/Windows/WindowsInput.h"

#ifndef BANAN_DISTRIBUTION
	#include <backends/imgui_impl_win32.h>
	extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

#include <windowsx.h>

namespace Banan
{
	
	HINSTANCE WindowsWindow::s_hInstance = NULL;

	LPWSTR CLASS_NAME = L"Window class";

	WindowsWindow::WindowsWindow(const std::string& title, uint32_t width, uint32_t height, bool vsync, const EventCallbackFn& callback)
	{
		m_data.title = title;
		m_data.width = width;
		m_data.height = height;
		m_data.vsync = vsync;
		m_data.eventCallback = callback;

		Init();

		m_initialized = true;
	}

	WindowsWindow::~WindowsWindow()
	{
		int status = ::ReleaseDC(m_data.hWnd, m_data.hDC);
		BANAN_ASSERT(status, "Could not release device context!");

		DestroyScope(m_renderContext);

		::DestroyWindow(m_data.hWnd);
		::UnregisterClassW(CLASS_NAME, s_hInstance);
	}

	void WindowsWindow::Init()
	{
		// Window Class
		WNDCLASS wc{};
		wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc		= WinProc;
		wc.hInstance		= s_hInstance;
		wc.hCursor			= ::LoadCursorW(NULL, IDC_ARROW);
		wc.lpszClassName	= CLASS_NAME;
		::RegisterClassW(&wc);

		// Create Window
		m_data.hWnd = ::CreateWindowExW(
			0,
			wc.lpszClassName,
			std::wstring(m_data.title.begin(), m_data.title.end()).c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, m_data.width, m_data.height,
			NULL,
			NULL,
			wc.hInstance,
			this
		);

		BANAN_ASSERT(m_data.hWnd, "Failed to create window!");

		::SetLastError(0);
		if (!::SetWindowLongPtrW(m_data.hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this)))
			BANAN_ASSERT(!::GetLastError(), "Failed to SetWindowLongPtrW");

		m_data.hDC = ::GetDC(m_data.hWnd);
		BANAN_ASSERT(m_data.hDC, "Could not get device context!");

		m_renderContext = RenderContext::Create(this);
		m_renderContext->Init();
		m_renderContext->SetVSync(m_data.vsync);

		::ShowWindow(m_data.hWnd, SW_SHOW);
	}

	void WindowsWindow::SetHInstance(HINSTANCE hInstace)
	{
		s_hInstance = hInstace;
	}


	void WindowsWindow::SetVSync(bool enable)
	{
		m_data.vsync = enable;
		m_renderContext->SetVSync(enable);
	}

	void WindowsWindow::SetTitle(const std::string& title)
	{
		m_data.title = title;
		::SetWindowTextW(m_data.hWnd, std::wstring(m_data.title.begin(), m_data.title.end()).c_str());
	}

	bool WindowsWindow::IsFocused() const
	{
		return ::GetFocus() == m_data.hWnd;
	}

	void WindowsWindow::OnUpdate(bool minimized)
	{
		MSG msg = { };
		while (::PeekMessageW(&msg, m_data.hWnd, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
		}

		if (!minimized)
		{
			WindowsInput::UpdateControllers();
			m_renderContext->SwapBuffers();
		}
	}

	LRESULT CALLBACK WindowsWindow::WinProc(HWND hWnd, UINT32 msg, WPARAM wParam, LPARAM lParam)
	{
#ifndef BANAN_DISTRIBUTION
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return true;
#endif

		WindowsWindow* window = reinterpret_cast<WindowsWindow*>(::GetWindowLongPtrW(hWnd, GWLP_USERDATA));;
		if (window) return window->m_WinProc(hWnd, msg, wParam, lParam);
		return ::DefWindowProcW(hWnd, msg, wParam, lParam);
	}

	LRESULT CALLBACK WindowsWindow::m_WinProc(HWND hWnd, UINT32 msg, WPARAM wParam, LPARAM lParam)
	{
		if (!m_initialized)
			return ::DefWindowProcW(hWnd, msg, wParam, lParam);

		switch (msg)
		{
			// ************** WINDOW EVENTS ****************
			case WM_CLOSE:
			{
				WindowCloseEvent e;
				m_data.eventCallback(e);
				break;
			}
			case WM_SIZE:
			{
				m_data.width = LOWORD(lParam);
				m_data.height = HIWORD(lParam);
				WindowResizeEvent e(m_data.width, m_data.height);
				m_data.eventCallback(e);
				break;
			}

			// ************** MOUSE EVENTS *****************
			case WM_MOUSEMOVE:
			{
				int xpos = GET_X_LPARAM(lParam);
				int ypos = GET_Y_LPARAM(lParam);
				MouseMoveEvent e(xpos, ypos);
				m_data.eventCallback(e);
				break;
			}
			case WM_MOUSEWHEEL:
			{
				int delta = GET_WHEEL_DELTA_WPARAM(wParam);
				MouseScrollEvent e((float)delta / (float)WHEEL_DELTA);
				m_data.eventCallback(e);
				break;
			}
			case WM_LBUTTONDOWN:
			{
				MousePressEvent e(MouseButton::ButtonLeft);
				m_data.eventCallback(e);
				break;
			}
			case WM_RBUTTONDOWN:
			{
				MousePressEvent e(MouseButton::ButtonRight);
				m_data.eventCallback(e);
				break;
			}
			case WM_MBUTTONDOWN:
			{
				MousePressEvent e(MouseButton::ButtonMiddle);
				m_data.eventCallback(e);
				break;
			}
			case WM_XBUTTONDOWN:
			{
				int button = GET_XBUTTON_WPARAM(wParam);
				if (button == XBUTTON1) {
					MousePressEvent e(MouseButton::Button3);
					m_data.eventCallback(e);
				}
				else if (button == XBUTTON2) {
					MousePressEvent e(MouseButton::Button4);
					m_data.eventCallback(e);
				}
				break;
			}
			case WM_LBUTTONUP:
			{
				MouseReleaseEvent e(MouseButton::ButtonLeft);
				m_data.eventCallback(e);
				break;
			}
			case WM_RBUTTONUP:
			{
				MouseReleaseEvent e(MouseButton::ButtonRight);
				m_data.eventCallback(e);
				break;
			}
			case WM_MBUTTONUP:
			{
				MouseReleaseEvent e(MouseButton::ButtonMiddle);
				m_data.eventCallback(e);
				break;
			}
			case WM_XBUTTONUP:
			{
				int button = GET_XBUTTON_WPARAM(wParam);
				if (button == XBUTTON1) {
					MouseReleaseEvent e(MouseButton::Button3);
					m_data.eventCallback(e);
				}
				else if (button == XBUTTON2) {
					MouseReleaseEvent e(MouseButton::Button4);
					m_data.eventCallback(e);
				}
				break;
			}

			// *************** KEY EVENTS ******************
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
			{
				int repeat = lParam & 0xff;
				KeyPressEvent e((Banan::KeyCode)wParam, repeat);
				m_data.eventCallback(e);
				break;
			}
			case WM_KEYUP:
			case WM_SYSKEYUP:
			{
				KeyReleaseEvent e((Banan::KeyCode)wParam);
				m_data.eventCallback(e);
				break;
			}
			case WM_CHAR:
			{
				KeyTypeEvent e((Banan::KeyCode)wParam);
				m_data.eventCallback(e);
				break;
			}

			default: return ::DefWindowProcW(hWnd, msg, wParam, lParam);
		}

		return 0;
	}

}