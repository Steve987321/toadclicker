#pragma once

class DoubleClicker
{
public:
	void StartThread();
	void StopThread();
	bool IsThreadAlive() const;

private:
	POINT m_pt = {};
	std::thread m_thread;
	std::thread m_thread2;
	std::atomic_bool m_threadFlag = false;
	std::shared_mutex m_mutex;
	int m_mouseDoubleClickingStage = 0;
	bool m_doOnceFlag = false;
	bool m_mouseFlag = false;
	bool m_isDoubleClicking = false;

	void Thread();
	void DoubleClick();
	void MouseDownCheckerThread();
};

inline auto p_doubleClicker = std::make_unique<DoubleClicker>();