#include "pch.h"
#include "Toad.h"
#include "Timer.h"

namespace toad
{

	Timer::Timer(bool start /*= false*/)
	{
		if (start)
			Start();
	}

	void Timer::Start()
	{
		m_start = std::chrono::high_resolution_clock::now();
	}

}