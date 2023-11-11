#pragma once

///
/// save, load or record click delays for playback 
/// 

enum class RECORDSTATUS
{
	AWAITING_FOR_CLICK,
	RECORDING,
	NOT_RECORDING,
	SKIPPING_NEXT_CLICK
};

class ClickRecorder
{

public:
	int is_first_click = 0;

	void playback_thread();
	void record_thread();

	// update global info variables
	void calc_vars();

	void start_playback_thread();
	void start_record_thread();

	void stop_playback_thread();
	void stop_record_thread();

	static ClickRecorder& get();

	void reset();

	// save current delay when recording 
	void save_delay();

	// load file with click delays 
	bool load_file(const std::string name, const std::string ext = ".txt");

	// save file with current loaded click delays 
	void save_file(const std::string name, const std::string ext = ".txt");

private:
	std::thread m_record_thread;
	std::thread m_playback_thread;

	std::atomic_bool m_is_recording = false;
	std::atomic_bool m_is_playback = false;

	int mouse_press_counter = 0;
	bool down = false;
	std::atomic_bool can_save = false;
	POINT pt = {};
	std::chrono::duration<float, std::milli> elapsed;
};
