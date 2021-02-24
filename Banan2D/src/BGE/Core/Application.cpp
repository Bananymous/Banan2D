#include "bgepch.h"
#include "Application.h"

#include "BGE/Renderer/Renderer2D.h"

#include "DeltaTimer.h"

namespace Banan
{
	Application* Application::s_instance = nullptr;

	Application::Application(const std::wstring& title, uint32_t width, uint32_t height, bool vsync)
	{
		s_instance = this;

		m_window = Window::Create(title, width, height, vsync, BGE_BIND_EVENT_FN(OnEvent));

		Renderer2D::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{

	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BGE_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BGE_BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_layers.rbegin(); it != m_layers.rend(); it++)
		{
			if (e.handled) break;
			(*it)->OnEvent(e);
		}

	}

	void Application::PushLayer(Layer* layer)
	{
		m_layers.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_layers.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::Close()
	{
		m_running = false;
	}

	void Application::Run()
	{
		if (m_running) return;

		m_running = true;

		DeltaTimer deltaTimer;
		while (m_running)
		{
			deltaTimer.Tick();

			if (!m_minimized)
			{
				for (Layer* layer : m_layers)
					layer->OnUpdate(deltaTimer.GetTime());

				m_ImGuiLayer->Begin();
				for (Layer* layer : m_layers)
					layer->OnImGuiRender();
				m_ImGuiLayer->End();
			}

			m_window->OnUpdate(m_minimized);
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		m_minimized = !e.GetWidth() || !e.GetHeight();
		Renderer2D::OnWindowResize(e.GetWidth(), e.GetHeight());
		return false;
	}

}