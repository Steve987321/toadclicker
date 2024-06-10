#pragma once

class Jitter
{
public:
	~Jitter();

	void DoJitter();

	void StartThread();
	void StopThread();

private:
	void MoveMouseX(int i);
	void MoveMouseY(int i);

	void Reset();
	void SetJitterPos();

	void Thread();

private:
	POINT m_origin = {};
	POINT m_pt = {};
	POINT m_dst = {};

	bool m_canJitter = true;
	bool m_canSetJitter = false;

	std::atomic_bool m_threadFlag;
	std::condition_variable m_cv;
	std::mutex m_mutex;
	std::thread m_thread;
};

inline auto p_Jitter = std::make_unique<Jitter>();