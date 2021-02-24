#pragma once

#include "BGE/Core/Input.h"

#include "BGE/Core/Application.h"

#include "WindowsWindow.h"

namespace Banan
{

	class WindowsInput : public Input
	{

	protected:
		virtual bool IsKeyPressedImpl(KeyCode keycode) override;

		virtual bool IsMouseButtonPressedImpl(MouseCode keycode) override;
		virtual std::pair<int, int> GetMousePosImpl() override;
		virtual int GetMouseXImpl() override;
		virtual int GetMouseYImpl() override;

	private:
		WindowsWindow& GetWindowsWindow() const { return static_cast<WindowsWindow&>(Application::Get().GetWindow()); }

	};

}