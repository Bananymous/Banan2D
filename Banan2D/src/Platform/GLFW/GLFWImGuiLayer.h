#pragma once

#include "Banan/ImGui/ImGuiLayer.h"

namespace Banan
{

	class GLFWImGuiLayer : public ImGuiLayer
	{
	public:
		GLFWImGuiLayer();

		virtual void OnAttach()			override;
		virtual void OnDetach()			override;
		virtual void OnEvent(Event& e)	override;

		virtual void Begin()			override;
		virtual void End()				override;

	private:
		bool m_blockEvents = true;
	};

}