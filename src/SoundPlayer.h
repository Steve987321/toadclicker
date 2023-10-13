#pragma once
class SoundPlayer
{
public:
	SoundPlayer();
	~SoundPlayer();

	void start_thread()
	{
		m_threadFlag = true;
		m_thread = std::thread{&SoundPlayer::thread, this};
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

	bool SetAudioDevice(int id);

	bool GetAllOutputDevices(std::vector<std::string>& vec);
	bool GetAudioDeviceVolume(int* val);
	void GetAllCompatibleSounds(std::vector<std::string>& vec, const std::vector<std::string>& vec_check) const;

private:

	int m_deviceId = 0;

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
};

inline auto p_SoundPlayer = std::make_unique<SoundPlayer>();