#include "bgepch.h"
#include "WindowsImGuiLayer.h"

#include "Banan/Core/Application.h"

#include "Platform/Windows/WindowsWindow.h"

#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_opengl3.h>


namespace Banan
{

	WindowsImGuiLayer::WindowsImGuiLayer() :
		ImGuiLayer("Windows ImGui Layer")
	{}

	void WindowsImGuiLayer::OnAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		WindowsWindow& window = static_cast<WindowsWindow&>(Application::Get().GetWindow());
		ImGui_ImplWin32_Init(window.GetHandle());
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void WindowsImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void WindowsImGuiLayer::OnEvent(Event& e)
	{
		if (m_blockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			e.handled |= e.IsInCategory(EventCategory::Mouse) & io.WantCaptureMouse;
			e.handled |= e.IsInCategory(EventCategory::Keyboard) & io.WantCaptureKeyboard;
		}
	}

	void WindowsImGuiLayer::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void WindowsImGuiLayer::End()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			WindowsWindow& window = static_cast<WindowsWindow&>(Application::Get().GetWindow());
			HGLRC backup_context = wglGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			wglMakeCurrent(window.GetDeviceContext(), backup_context);
		}
	}

}