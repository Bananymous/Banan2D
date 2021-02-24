#pragma once

#include "BGE/Core/Layer.h"

#include "BGE/Event/MouseEvent.h"
#include "BGE/Event/KeyEvent.h"

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