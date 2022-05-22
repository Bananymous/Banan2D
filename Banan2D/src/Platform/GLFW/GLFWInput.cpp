#include "bgepch.h"
#include "GLFWInput.h"

#include "Banan/Core/Input.h"
#include "Banan/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Banan
{
	Input* Input::s_instance = new GLFWInput;

	bool GLFWInput::IsKeyPressedImpl(KeyCode keycode)
	{
		GLFWwindow* window = GetGLFWWindowHandle();
		int state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool GLFWInput::IsMouseButtonPressedImpl(MouseCode button)
	{
		GLFWwindow* window = GetGLFWWindowHandle();
		int state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	std::pair<int, int> GLFWInput::GetMousePosImpl()
	{
		GLFWwindow* window = GetGLFWWindowHandle();
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (int)xpos, (int)ypos };
	}

	int GLFWInput::GetMouseXImpl()
	{
		auto [x, y] = GetMousePosImpl();
		return x;
	}

	int GLFWInput::GetMouseYImpl()
	{
		auto [x, y] = GetMousePosImpl();
		return y;
	}

	bool GLFWInput::IsControllerConnectedImpl(int controller)
	{
		return false;
	}

	bool GLFWInput::IsControllerButtonPressedImpl(int controller, ControllerCode button)
	{
		return false;
	}

	float GLFWInput::GetControllerLeftTriggerImpl(int controller)
	{
		return 0.0f;
	}

	float GLFWInput::GetControllerRightTriggerImpl(int controller)
	{
		return 0.0f;
	}

	std::pair<float, float> GLFWInput::GetControllerLeftStickImpl(int controller)
	{
		return { 0.0f, 0.0f };
	}

	std::pair<float, float> GLFWInput::GetControllerRightStickImpl(int controller)
	{
		return { 0.0f, 0.0f };
	}

}