#pragma once

class c_doubleClicker
{
private:
	POINT pt = {};
	std::thread m_thread;
	std::atomic_bool thread_flag = false;
	std::shared_mutex mutex;
	bool mouseFlag = false;

	void thread();
	void double_click();
public:
	void start_thread()
	{
		thread_flag = true;
		m_thread = std::thread{&c_doubleClicker::thread, this};
	}
	void stop_thread()
	{
		thread_flag = false;
		if (m_thread.joinable()) m_thread.join();
	}
	~c_doubleClicker()
	{
		stop_thread();
	}
};

inline auto p_doubleClicker = std::make_unique<c_doubleClicker>();