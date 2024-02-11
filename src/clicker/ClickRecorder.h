#pragma once

class ClickRecorder
{
public:
	int isFirst_click = 0;

	void calcVars();
	void init_playback_thread();
	void init_record_thread();
	void reset();
	void save_delay();
	void playback_thread();
	void vars_check_thread();

	bool load_file(const std::string name, const std::string ext = ".txt");
	void save_file(const std::string name, const std::string ext = ".txt");
private:
	int k = 0;
	bool down = false;
	bool can_save = false;
	POINT pt = {};
	std::chrono::duration<float, std::milli> elapsed{};
};

inline auto p_clickRecorder = std::make_unique<ClickRecorder>();