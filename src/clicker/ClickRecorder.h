#pragma once

class c_clickRecorder
{
private:
	int k = 0; 
	bool down = false;
	POINT pt = {};
public:
	int isFirst_click = 0;

	void init_thread();
	void reset();
	void save_delay();
	void playback_thread();

	bool load_file(const std::string name, const std::string ext = ".txt");
	void save_file(const std::string name, const std::string ext = ".txt");

};
inline auto p_clickRecorder = std::make_unique<c_clickRecorder>();