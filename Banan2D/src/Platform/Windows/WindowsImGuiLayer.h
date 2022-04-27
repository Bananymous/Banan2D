#pragma once

#include "../../Banan/ImGui/ImGuiLayer.h"

namespace Banan
{

	class WindowsImGuiLayer : public ImGuiLayer
	{
	public:
		WindowsImGuiLayer();

		virtual void OnAttach()			override;
		virtual void OnDetach()			override;
		virtual void OnEvent(Event& e)	override;

		void Begin();
		void End();

	private:
		bool m_blockEvents = true;
	};

}