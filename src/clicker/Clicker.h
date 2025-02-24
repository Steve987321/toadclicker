#pragma once

class LeftClicker {
public:
	void StartThread();
	void StopThread();
    bool IsThreadAlive() const;

protected:
	enum class mouse_type : uint8_t
	{
		LEFT,
		RIGHT
	};

	void ResetVars();

	void SendDown(mouse_type mb, POINT& pt, float& sometingdelay, float delayclick2);
	void SendUp(mouse_type mb, POINT& pt, float& sometingdelay, float delayclick2);

	void Thread();

protected:
	std::thread m_thread;
	std::atomic_bool m_threadFlag;

	size_t selected_clicksound = 0;

	// #TODO Abstract rand
	int boost_counter = 0;

	int boost_counter2 = 0;
	bool boost = false;
	bool boost2 = false;
	int counter = 0;
	int counter2 = 0;
	bool bdrop = false;

	bool first_click = false;

	bool inconsistensy = false;
	bool inconsistensy2 = false;
	int inconsistensycounter = 0;
	int inconsistensycounter2 = 0;

	float min = 0.4f;
	float max = 0.8f;
	bool bonce = false;
	int spikemaxchance = 10;
	int chancespike = 0;

	float mincanboostchance = 12.f;
	float mincandropchance = 12.f;

	bool can_stop = true;
};

inline auto p_clicker = std::make_unique<LeftClicker>();

//right clicker
class RightClicker : private LeftClicker {
public:
    void StartThread();
    void StopThread();
    bool IsThreadAlive() const;
private:
	void ThreadRight();
};

inline auto p_rightClicker = std::make_unique<RightClicker>();