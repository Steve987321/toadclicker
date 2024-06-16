#pragma once

class DoubleClicker
{
public:
	void StartThread();
	void StopThread();
	bool IsThreadAlive() const;

private:
	void Thread();
	void MouseDownCheckerThread();
	void DoubleClick();

private:
	POINT m_pt = {};
	std::thread m_thread;
	std::thread m_checkerThread;
	std::atomic_bool m_threadFlag = false;
	std::mutex m_mutex;
	std::condition_variable m_cv;

	int m_mouseDoubleClickingStage = 0;
	bool m_doOnceFlag = false;
	bool m_mouseFlag = false;
	bool m_isDoubleClicking = false;
};

inline auto p_doubleClicker = std::make_unique<DoubleClicker>();