#pragma once

#include "KeyCode.h"
#include "MouseCode.h"
#include "ControllerCode.h"

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

		class Controller
		{
		public:
			static bool IsConnected(int controller)								{ return s_instance->IsControllerConnectedImpl(controller); }
			static bool IsButtonPressed(int controller, ControllerCode button)	{ return s_instance->IsControllerButtonPressedImpl(controller, button); }
			static float GetLeftTrigger(int controller)							{ return s_instance->GetControllerLeftTriggerImpl(controller); }
			static float GetRightTrigger(int controller)						{ return s_instance->GetControllerRightTriggerImpl(controller); }
			static std::pair<float, float> GetLeftStick(int controller)			{ return s_instance->GetControllerLeftStickImpl(controller); }
			static std::pair<float, float> GetRightStick(int controller)		{ return s_instance->GetControllerRightStickImpl(controller); }
		};

	protected:
		virtual bool IsKeyPressedImpl(KeyCode keycode) = 0;

		virtual bool IsMouseButtonPressedImpl(MouseCode keycode) = 0;
		virtual std::pair<int, int> GetMousePosImpl() = 0;
		virtual int GetMouseXImpl() = 0;
		virtual int GetMouseYImpl() = 0;

		virtual bool IsControllerConnectedImpl(int controller) = 0;
		virtual bool IsControllerButtonPressedImpl(int controller, ControllerCode button) = 0;
		virtual float GetControllerLeftTriggerImpl(int controller) = 0;
		virtual float GetControllerRightTriggerImpl(int controller) = 0;
		virtual std::pair<float, float> GetControllerLeftStickImpl(int controller) = 0;
		virtual std::pair<float, float> GetControllerRightStickImpl(int controller) = 0;

	private:
		static Input* s_instance;

	};

}