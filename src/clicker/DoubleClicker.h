#pragma once

class c_doubleClicker
{
private:
	POINT pt = {};
	std::thread m_thread, m_thread2;
	std::atomic_bool thread_flag = false;
	std::shared_mutex mutex;
	bool do_onceFlag = false;
	bool mouseFlag = false;
	int mouseDoubleClickingStage = false;
	bool isDoubleClicking = false;

	void thread();
	void double_click();
	void mousedownchecker_thread();
public:
	void start_thread()
	{
		thread_flag = true;
		m_thread = std::thread{&c_doubleClicker::thread, this};
		m_thread2 = std::thread{&c_doubleClicker::mousedownchecker_thread, this};
	}
	void stop_thread()
	{
		thread_flag = false;
		if (m_thread.joinable()) m_thread.join();
		if (m_thread2.joinable()) m_thread2.join();
	}

	bool is_thread_alive() const
	{
		return thread_flag;
	}
};

inline auto p_doubleClicker = std::make_unique<c_doubleClicker>();