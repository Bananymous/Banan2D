#pragma once

#include "../../Banan/Core/Input.h"
#include "../../Banan/Core/Application.h"

#include "./GLFWWindow.h"

namespace Banan
{

	class GLFWInput : public Input
	{
	protected:
		virtual bool IsKeyPressedImpl(KeyCode keycode) override;

		virtual bool IsMouseButtonPressedImpl(MouseCode keycode) override;
		virtual std::pair<int, int> GetMousePosImpl() override;
		virtual int GetMouseXImpl() override;
		virtual int GetMouseYImpl() override;

		virtual bool IsControllerConnectedImpl(int controller) override;
		virtual bool IsControllerButtonPressedImpl(int controller, ControllerCode button) override;
		virtual float GetControllerLeftTriggerImpl(int controller) override;
		virtual float GetControllerRightTriggerImpl(int controller) override;
		virtual std::pair<float, float> GetControllerLeftStickImpl(int controller) override;
		virtual std::pair<float, float> GetControllerRightStickImpl(int controller) override;

	private:
		GLFWwindow* GetGLFWWindowHandle() const { return static_cast<GLFWWindow&>(Application::Get().GetWindow()).GetWindowHandle(); }
	};

}