#pragma once

#include "./Timestep.h"

#include <chrono>

namespace Banan
{

	class DeltaTimer
	{
		using clock = std::chrono::high_resolution_clock;
		using duration = std::chrono::duration<float>;

	public:
		void Tick()
		{
			clock::time_point current = clock::now();
			m_delta = current - m_lastTime;
			m_lastTime = current;
		}

		Timestep GetTime() const { return Timestep(m_delta.count()); }

	private:
		clock::time_point m_lastTime	= clock::now();
		duration m_delta				= std::chrono::milliseconds(0);
	};

}