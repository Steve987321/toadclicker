#pragma once

// non owning buffer for the sound 
struct SoundBuffer 
{
	void* buf = nullptr;
	DWORD size = 0;
};

// play sound using mmeapi
class SoundPlayer
{
public:
	SoundPlayer();
	~SoundPlayer();

public:
	void StartThread();
	void StopThread();
	bool IsThreadAlive() const;

	void LoadSound(const std::filesystem::path& sound_file);
	void UnloadSound(const std::filesystem::path& sound_file);

	// play sound by given sound file
	void TriggerSoundPlay(std::string_view sound_file);

	void SetDeviceID(uint32_t id);

	bool GetAllOutputDevices(std::vector<std::string>& vec) const;
	void GetAllCompatibleSounds(std::vector<std::string>& vec) const;

private:
	// returns allocated sound buffer 
	SoundBuffer LoadAudioBlockNew(std::filesystem::path sound_file);

	void WriteAudioBlock();
	void Thread();
	void Reset();
	bool PlaySound();

	HWAVEOUT m_hWaveOut = {};
	WAVEFORMATEX m_format = {};
	WAVEHDR m_header{};

	bool m_play = false;
	bool m_resetWaveOut = false;
	std::thread m_thread;
	std::atomic_bool m_threadFlag = false;
	std::condition_variable m_cv;
	std::mutex m_mutex;

	SoundBuffer* m_currentSoundBuf = nullptr;
	std::unordered_map<std::string, SoundBuffer> m_soundBuffers;
	DWORD m_vol = 0xFFFF;
};

inline auto p_SoundPlayer = std::make_unique<SoundPlayer>();