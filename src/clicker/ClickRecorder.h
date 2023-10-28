#pragma once

///
/// save, load or record click delays for playback 
/// 
class ClickRecorder
{
public:
	int is_first_click = 0;

	void playback_thread();
	void record_thread();

	// update global info variables
	void calcVars();

	void start_playback_thread();
	void start_record_thread();

	void reset();

	// save current delay when recording 
	void save_delay();


	// load file with click delays 
	bool load_file(const std::string name, const std::string ext = ".txt");

	// save file with current loaded click delays 
	void save_file(const std::string name, const std::string ext = ".txt");

private:
	int mouse_press_counter = 0;
	bool down = false;
	bool can_save = false;
	POINT pt = {};
	std::chrono::duration<float, std::milli> elapsed;
};

inline auto p_clickRecorder = std::make_unique<ClickRecorder>();