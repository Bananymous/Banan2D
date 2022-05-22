#include "bgepch.h"
#include "WindowsInput.h"

#include <Xinput.h>

#include <chrono>

namespace Banan
{

	Input* Input::s_instance = new WindowsInput;

	static XINPUT_STATE	s_controllerState[4]{};
	static bool			s_controllerConnected[4]{};
	

	bool WindowsInput::IsKeyPressedImpl(KeyCode keycode)
	{
		WindowsWindow& window = GetWindowsWindow();
		return window.IsFocused() && GetKeyState(keycode) >> 8;
	}

	bool WindowsInput::IsMouseButtonPressedImpl(MouseCode button)
	{
		WindowsWindow& window = GetWindowsWindow();
		return window.IsFocused() && GetKeyState(button) >> 8;
	}

	std::pair<int, int> WindowsInput::GetMousePosImpl()
	{
		WindowsWindow& window = GetWindowsWindow();
		POINT mousePos;
		GetCursorPos(&mousePos);
		ScreenToClient(window.GetHandle(), &mousePos);
		return { mousePos.x, mousePos.y };
	}

	int WindowsInput::GetMouseXImpl()
	{
		auto [x, y] = GetMousePosImpl();
		return x;
	}

	int WindowsInput::GetMouseYImpl()
	{
		auto [x, y] = GetMousePosImpl();
		return y;
	}


	bool WindowsInput::IsControllerConnectedImpl(int controller)
	{
		return s_controllerConnected[controller];
	}

	bool WindowsInput::IsControllerButtonPressedImpl(int controller, ControllerCode button)
	{
		return s_controllerState[controller].Gamepad.wButtons & button;
	}

	float WindowsInput::GetControllerLeftTriggerImpl(int controller)
	{
		return (float)s_controllerState[controller].Gamepad.bLeftTrigger / 255.0f;
	}

	float WindowsInput::GetControllerRightTriggerImpl(int controller)
	{
		return (float)s_controllerState[controller].Gamepad.bRightTrigger / 255.0f;
	}

	std::pair<float, float> WindowsInput::GetControllerLeftStickImpl(int controller)
	{
		float xoff = s_controllerState[controller].Gamepad.sThumbLX / 32768.0f;
		float yoff = s_controllerState[controller].Gamepad.sThumbLY / 32768.0f;
		return { xoff, yoff };
	}

	std::pair<float, float> WindowsInput::GetControllerRightStickImpl(int controller)
	{
		float xoff = s_controllerState[controller].Gamepad.sThumbRX / 32768.0f;
		float yoff = s_controllerState[controller].Gamepad.sThumbRY / 32768.0f;
		return { xoff, yoff };
	}

	void WindowsInput::UpdateControllers()
	{
		std::memset(&s_controllerState, 0, sizeof(s_controllerState));

		bool updateAll = false;

		constexpr auto updateFreq = std::chrono::milliseconds(100);

		static auto last_query = std::chrono::system_clock::now();
		auto current_time = std::chrono::system_clock::now();

		if (current_time - last_query >= updateFreq)
		{
			updateAll = true;
			last_query = current_time;
		}

		for (uint32_t i = 0; i < 4; i++)
		{
			if (updateAll || s_controllerConnected[i])
			{
				DWORD result;
				result = XInputGetState(i, &s_controllerState[i]);
				s_controllerConnected[i] = (result == ERROR_SUCCESS);
			}
		}
	}

}