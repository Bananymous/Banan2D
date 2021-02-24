#pragma once

#include "Timestep.h"
#include <chrono>

namespace Banan
{
	using namespace std::chrono;
	using clock = high_resolution_clock;

	class DeltaTimer
	{
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
		duration<float> m_delta			= 0ms;
	};

}