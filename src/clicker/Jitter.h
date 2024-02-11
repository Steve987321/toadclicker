#pragma once
class Jitter
{
public:
	void Reset();
	void SetJitterPos();
	void Thread();
	bool can_setJitter = false;

private:
	POINT m_origin = {};
	POINT m_pt = {};
	POINT m_dst = {};

	bool m_canJitter = true;
	void MoveMouseX(int i);
	void MoveMouseY(int i);
};

inline auto p_Jitter = std::make_unique<Jitter>();