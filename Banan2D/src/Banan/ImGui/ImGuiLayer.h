#pragma once

#include "Banan/Core/Layer.h"

#include "Banan/Event/MouseEvent.h"
#include "Banan/Event/KeyEvent.h"

namespace Banan {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach()			override;
		virtual void OnDetach()			override;
		virtual void OnEvent(Event& e)	override;

		void Begin();
		void End();

	private:
		bool m_blockEvents = true;

	};

}