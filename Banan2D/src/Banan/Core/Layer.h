#pragma once

#include "Banan/Event/Event.h"

#include "Banan/Core/Timestep.h"

#include <string>
#include <vector>

namespace Banan
{

	class Layer
	{
	public:
		Layer(const std::string& name = "Layer") :
			m_debugName(name)
		{ }

		virtual ~Layer() = default;

		virtual void OnAttach()				{ }
		virtual void OnDetach()				{ }
		virtual void OnUpdate(Timestep ts)	{ }
		virtual void OnImGuiRender()		{ }
		virtual void OnEvent(Event& e)		{ }

		const std::string& GetName() const { return m_debugName; }

	protected:
		std::string m_debugName;
	};


	class LayerStack
	{
	public:
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		std::vector<Layer*>::iterator begin()			{ return m_layers.begin(); }
		std::vector<Layer*>::iterator end()				{ return m_layers.end(); }
		std::vector<Layer*>::reverse_iterator rbegin()	{ return m_layers.rbegin(); }
		std::vector<Layer*>::reverse_iterator rend()	{ return m_layers.rend(); }

	private:
		std::vector<Layer*> m_layers;
		uint32_t m_lastInsertIndex = 0;
	};
}