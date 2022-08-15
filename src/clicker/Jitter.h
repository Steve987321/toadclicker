#pragma once
class Jitter
{
private:
	POINT origin = {}, pt = {}, dst = {};
	bool can_jitter = true;
	void move_mouseX(int i);
	void move_mouseY(int i);
public:
	void reset();
	void set_jitter_pos();
	void thread();
};

inline auto p_Jitter = std::make_unique<Jitter>();