#pragma once

#include "KeyCode.h"
#include "MouseCode.h"

#include <utility>

namespace Banan
{

	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode keycode) { return s_instance->IsKeyPressedImpl(keycode); }

		static bool IsMouseButtonPressed(MouseCode button)	{ return s_instance->IsMouseButtonPressedImpl(button); }
		static std::pair<int, int> GetMousePos()			{ return s_instance->GetMousePosImpl(); };
		static int GetMouseX()								{ return s_instance->GetMouseXImpl(); };
		static int GetMouseY()								{ return s_instance->GetMouseYImpl(); };

	protected:
		virtual bool IsKeyPressedImpl(KeyCode keycode) = 0;

		virtual bool IsMouseButtonPressedImpl(MouseCode keycode) = 0;
		virtual std::pair<int, int> GetMousePosImpl() = 0;
		virtual int GetMouseXImpl() = 0;
		virtual int GetMouseYImpl() = 0;

	private:
		static Input* s_instance;

	};

}