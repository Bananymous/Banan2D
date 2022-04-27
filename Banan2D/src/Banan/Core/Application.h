#pragma once

#include "./Window.h"
#include "./Layer.h"

#include "../Event/WindowEvent.h"

#ifdef BANAN_DISTRIBUTION
	#ifndef BANAN_DISABLE_IMGUI
		#define BANAN_DISABLE_IMGUI
	#endif
#endif

#define BANAN_DISABLE_IMGUI

#ifndef BANAN_DISABLE_IMGUI 
#include "../ImGui/ImGuiLayer.h"
#endif

namespace Banan
{

	class Application
	{
	public:
		Application(const std::wstring& title, uint32_t width, uint32_t height, bool vsync = true);
		virtual ~Application();

		Application(const Application&) = delete;
		void operator=(const Application&) = delete;

		void Close();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		static Application& Get() { return *s_instance; }
		Window& GetWindow() const { return *m_window; }

	public:
		void Run();

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		bool m_running = false;
		bool m_minimized = false;

		Scope<Window> m_window;

#ifndef BANAN_DISABLE_IMGUI
		ImGuiLayer* m_ImGuiLayer;
#endif
		LayerStack m_layers;

	private:
		static Application* s_instance;

		friend BANAN_MAIN;
	};

	Application* CreateApplication();

}