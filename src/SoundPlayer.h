#pragma once

class SoundPlayer
{
public:
	SoundPlayer();
	~SoundPlayer();

public:
	void StartThread();
	void StopThread();
	bool IsThreadAlive() const;

	void TriggerSoundPlay();

	bool GetAllOutputDevices(std::vector<std::string>& vec);
	void GetAllCompatibleSounds(std::vector<std::string>& vec, const std::vector<std::string>& vec_check) const;

private:
	bool LoadAudioBlockNew();
	void WriteAudioBlock();
	void Thread();
	void Reset();
	bool PlaySound();

	HWAVEOUT m_hWaveOut = {};
	WAVEFORMATEX m_format = {};
	WAVEHDR m_header{};

	bool m_play = false;
	bool m_resetOnce = false;
	std::thread m_thread;
	std::atomic_bool m_threadFlag = false;
	std::condition_variable m_cv;
	std::mutex m_mutex;
	DWORD m_size = NULL;
	void* m_block = nullptr;

	DWORD m_vol = 0xFFFF;
};

inline auto p_SoundPlayer = std::make_unique<SoundPlayer>();