#include "bgepch.h"
#include "LinuxInput.h"

namespace Banan
{

    Input* Input::s_instance = new LinuxInput;

    bool LinuxInput::IsKeyPressedImpl(KeyCode keycode)
    {
        return false;
    }

	bool LinuxInput::IsMouseButtonPressedImpl(MouseCode keycode)
    {
        return false;
    }
	
    std::pair<int, int> LinuxInput::GetMousePosImpl()
    {
        return { 0, 0 };
    }
	
    int LinuxInput::GetMouseXImpl()
    {
        return 0;
    }
	
    int LinuxInput::GetMouseYImpl()
    {
        return 0;
    }
	
    bool LinuxInput::IsControllerConnectedImpl(int controller)
    {
        return false;
    }
	
    bool LinuxInput::IsControllerButtonPressedImpl(int controller, ControllerCode button)
    {
        return false;
    }
	
    float LinuxInput::GetControllerLeftTriggerImpl(int controller)
    {
        return 0.f;
    }
	
    float LinuxInput::GetControllerRightTriggerImpl(int controller)
    {
        return 0.f;
    }
	
    std::pair<float, float> LinuxInput::GetControllerLeftStickImpl(int controller)
    {
        return {0.f, 0.f};
    }

	std::pair<float, float> LinuxInput::GetControllerRightStickImpl(int controller)
    {
        return {0.f, 0.f};
    }
	
}