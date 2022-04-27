#include "bgepch.h"
#include "Layer.h"

#include <algorithm>

namespace Banan
{

	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_layers) {
			layer->OnDetach();
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_layers.emplace(m_layers.begin() + m_lastInsertIndex, layer);
		m_lastInsertIndex++;
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		m_layers.emplace_back(overlay);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto it = std::find(m_layers.begin(), m_layers.begin() + m_lastInsertIndex, layer);
		if (it != m_layers.begin() + m_lastInsertIndex)
		{
			layer->OnDetach();
			m_layers.erase(it);
			m_lastInsertIndex--;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		auto it = std::find(m_layers.begin() + m_lastInsertIndex, m_layers.end(), overlay);
		if (it != m_layers.end())
		{
			overlay->OnDetach();
			m_layers.erase(it);
		}
	}

}