#pragma once

//
class ClickRecorder
{
public:
	~ClickRecorder();

public:
	void CalcVars();
	void Reset();
	void SaveDelay();

	bool LoadFile(const std::string& name, const std::string& ext = ".txt");
	void SaveFile(const std::string& name, const std::string& ext = ".txt");

public:
	void StartPlaybackThread();
	void StopPlaybackThread();
	bool IsPlaybackThreadAlive() const;

	void StartRecordingThread();
	void StopRecordingThread();
	bool IsRecordingThreadAlive() const;

private:
	void PlaybackThread();
	void VarsCheckThread();

private:
	int isFirst_click = 0;
	int k = 0;
	bool down = false;
	bool can_save = false;
	POINT pt = {};
	std::chrono::duration<float, std::milli> elapsed{};

private:
	std::thread m_playbackThread, m_recordThread;
	std::mutex m_playbackMutex, m_recordMutex;
	std::condition_variable m_playbackCV, m_recordCV;
	std::atomic_bool m_playbackFlag = false, m_recordFlag = false;
};

inline auto p_clickRecorder = std::make_unique<ClickRecorder>();