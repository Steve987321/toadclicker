#pragma once

namespace toad
{

class Timer
{
public:
	explicit Timer(bool start = false);

public:
	/// Starts or Resets the begin point of the timer
	void Start();

	/// Returns the time interval between m_start and now.
	///
	/// DurationT specifies the time metric 
	template<class DurationT = std::chrono::milliseconds>
	float Elapsed() const
	{
		return std::chrono::duration_cast<DurationT>(std::chrono::high_resolution_clock::now() - m_start).count();
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
};

}
