#pragma once

class c_clickRecorder
{
private:
	int k = 0; 
	bool down = false;
	bool can_save = false;
	POINT pt = {};
	std::chrono::duration<float, std::milli> elapsed;
public:
	int isFirst_click = 0;

	void init_playback_thread();
	void init_record_thread();
	void reset();
	void save_delay();
	void playback_thread();
	void vars_check_thread();

	bool load_file(const std::string name, const std::string ext = ".txt");
	void save_file(const std::string name, const std::string ext = ".txt");

};
inline auto p_clickRecorder = std::make_unique<c_clickRecorder>();