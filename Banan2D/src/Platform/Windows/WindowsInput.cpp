#include "bgepch.h"
#include "WindowsInput.h"

namespace Banan
{

	Input* Input::s_instance = new WindowsInput;

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

}