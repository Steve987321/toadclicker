#pragma once
class SoundPlayer
{
public:
	SoundPlayer();
	~SoundPlayer();

	void start_thread();
	void stop_thread();
	bool is_thread_alive() const;

	/// Caches a list of files
	/// Also clears it before 
	/// @param files list of strings that contains the full path to the audio file (.wav or .raw)
	bool CacheAudioFiles(std::vector<std::string>& files);

	/// Caches an audio file 
	bool CacheAudioFile(std::string_view file_path);

	// Clears cached files 
	bool ClearCachedFile(std::string_view file_path);

	bool SetAudioDevice(int id);

	// sets playback rate of the current audio device 
	void SetPlayBackRate(float multiplier);

	bool GetAllOutputDevices(std::vector<std::string>& vec);
	void GetAllCompatibleSounds(std::vector<std::string>& vec, const std::vector<std::string>& vec_check) const;

private:
	int m_deviceId = 0;

	HWAVEOUT m_hWaveOut = {};
	WAVEFORMATEX m_format = {};

	std::thread m_thread;
	std::atomic_bool m_threadFlag = false;

	// stores cached audio files 
	std::unordered_map<std::string, WAVEHDR> m_cached = {};

	DWORD m_vol = 0xFFFF;

	DWORD to_dword_multiplier(float multiplier);

	void writeAudioBlock(WAVEHDR* data);
	void thread();
	void reset();
	bool play_sound();
};

inline auto p_SoundPlayer = std::make_unique<SoundPlayer>();