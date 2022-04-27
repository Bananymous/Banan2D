#pragma once

#include <cstdint>

namespace Banan
{

	using MouseCode = uint8_t;

	namespace MouseButton
	{
		enum : MouseCode
		{
			Button0 = 1,
			Button1 = 2,
			Button2 = 4,
			Button3 = 5,
			Button4 = 6,

			ButtonLeft = Button0,
			ButtonRight = Button1,
			ButtonMiddle = Button2
		};
	}
	
}