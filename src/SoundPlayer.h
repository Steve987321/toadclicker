#pragma once
class c_SoundPlayer
{
private:
//	std::mutex m_mutex;

	HWAVEOUT m_hWaveOut = {};
	WAVEFORMATEX m_format = {};
	WAVEHDR m_header{};

	bool m_onceFlag = false;
	std::thread m_thread;
	std::atomic_bool m_threadFlag = false;
	DWORD m_size = NULL;
	void* m_block = nullptr;

	DWORD m_vol = 0xFFFF;

	bool loadAudioBlockNew();
	void writeAudioBlock();
	void thread();
	void reset();
	bool play_sound();
public:
	// init
	c_SoundPlayer();
	void start_thread()
	{
		m_threadFlag = true;
		m_thread = std::thread{&c_SoundPlayer::thread, this};
	}
	void stop_thread()
	{
		m_threadFlag = false;
		m_thread.join();
	}

	bool is_thread_alive() const
	{
		return m_threadFlag;
	}

	bool get_all_outputDevices(std::vector<std::string>& vec);
	bool get_audioDevVol(int* val);

	void get_all_compatible_sounds(std::vector<std::string>& vec, const std::vector<std::string>& vec_check) const;
};

inline auto p_SoundPlayer = std::make_unique<c_SoundPlayer>();