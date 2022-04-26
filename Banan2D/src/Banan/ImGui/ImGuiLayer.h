#pragma once

#include "Banan/Core/Layer.h"

namespace Banan
{

	class ImGuiLayer : public Layer
	{
	protected:
		ImGuiLayer(const std::string& name) : Layer(name) {}

	public:
		virtual void Begin() = 0;
		virtual void End()   = 0;

		static ImGuiLayer* Create();
	};

}