#pragma once

#include "Window.h"
#include "Layer.h"
#include "BGE/ImGui/ImGuiLayer.h"

#include "BGE/Event/WindowEvent.h"

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

	private:
		void Run();

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		bool m_running = false;
		bool m_minimized = false;

		Scope<Window> m_window;

		ImGuiLayer* m_ImGuiLayer;
		
		LayerStack m_layers;

	private:
		static Application* s_instance;

		friend BGE_MAIN;
	};

	Application* CreateApplication();

}