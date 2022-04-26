#include "bgepch.h"
#include "Application.h"

#include "Banan/Renderer/Renderer2D.h"

#include "DeltaTimer.h"

namespace Banan
{
	Application* Application::s_instance = nullptr;

	Application::Application(const std::wstring& title, uint32_t width, uint32_t height, bool vsync)
	{
		s_instance = this;

		m_window = Window::Create(title, width, height, vsync, BANAN_BIND_EVENT_FN(Application::OnEvent));

		Renderer2D::Init();

#ifndef BANAN_DISTRIBUTION
		m_ImGuiLayer = ImGuiLayer::Create();
		PushOverlay(m_ImGuiLayer);
#endif
	}

	Application::~Application()
	{
		Renderer2D::Shutdown();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BANAN_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BANAN_BIND_EVENT_FN(Application::OnWindowResize));

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

#ifndef BANAN_DISTRIBUTION
				m_ImGuiLayer->Begin();
				for (Layer* layer : m_layers)
					layer->OnImGuiRender();
				m_ImGuiLayer->End();
#endif
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
		if (m_minimized)
			return false;
		Renderer2D::OnWindowResize(e.GetWidth(), e.GetHeight());
		return false;
	}

}