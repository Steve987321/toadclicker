#include "pch.h"
#include "SoundPlayer.h"
#include "Toad.h"

namespace fs = std::filesystem;

SoundPlayer::SoundPlayer()
{
	ZeroMemory(&m_format, sizeof(WAVEFORMATEX));
	m_format.wFormatTag = WAVE_FORMAT_PCM;
	m_format.nChannels = 2;
	m_format.wBitsPerSample = 16;
	m_format.nSamplesPerSec = 44100;
	m_format.nBlockAlign = m_format.nChannels * m_format.wBitsPerSample / 8;
	m_format.nAvgBytesPerSec = m_format.nSamplesPerSec * m_format.nBlockAlign;
	m_format.cbSize = 0;
}

SoundPlayer::~SoundPlayer()
{
	StopThread();

	for (auto& [name, sound_buf] : m_soundBuffers)
	{
		if (sound_buf.buf)
			free(sound_buf.buf);
	}
};

void SoundPlayer::StartThread()
{
	m_threadFlag = true;
	m_thread = std::thread{ &SoundPlayer::Thread, this };
}

void SoundPlayer::StopThread()
{
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_threadFlag = false;
	}

	m_cv.notify_one();

	if (m_thread.joinable())
		m_thread.join();
}

bool SoundPlayer::IsThreadAlive() const
{
	return m_threadFlag;
}

void SoundPlayer::LoadSound(const std::filesystem::path& sound_file)
{
	LOG_DEBUGF("[SoundPlayer] Preparing sound for playback: %s", sound_file.string().c_str());

	m_soundBuffers[sound_file.string()] = LoadAudioBlockNew(sound_file);
}

void SoundPlayer::UnloadSound(const std::filesystem::path& sound_file)
{
	auto it = m_soundBuffers.find(sound_file.string());
	if (it != m_soundBuffers.end())
	{
		if (m_currentSoundBuf && m_currentSoundBuf->buf == it->second.buf)
			m_currentSoundBuf = nullptr;

		free(it->second.buf);
		m_soundBuffers.erase(it);
	}
	else
	{
		LOG_ERRORF("[SoundPlayer] Can't unload file because it doesn't exist: %s", sound_file.c_str());
	}
}

void SoundPlayer::TriggerSoundPlay(std::string_view sound_file)
{
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if (toad::clicksounds::selected_device == "none" || !toad::clicksounds::use_custom_output)
		{
			PlaySoundA(sound_file.data(), NULL, SND_FILENAME | SND_NODEFAULT | SND_ASYNC);
			return;
		}

		auto it = m_soundBuffers.find(sound_file.data());
		if (it == m_soundBuffers.end())
		{
			LOG_ERRORF("[SoundPlayer] Sound can't be played because it hasn't been loaded: %s", sound_file.data());
			return;
		}

		m_currentSoundBuf = &(it->second);
		m_play = true;
	}

	m_cv.notify_one();
}

void SoundPlayer::SetDeviceID(uint32_t id)
{
	MMRESULT res = waveOutOpen(&m_hWaveOut, toad::clicksounds::selected_device_ID, &m_format, NULL, NULL, CALLBACK_NULL);
	if (res != MMSYSERR_NOERROR)
	{
		LOG_ERRORF("[SoundPlayer] wave out open error: %d", res);
	}
}

bool SoundPlayer::GetAllOutputDevices(std::vector<std::string>& vec) const
{
	vec.clear();
	for (uint32_t i = 0; i < waveOutGetNumDevs(); i++)
	{
		WAVEOUTCAPS wave;
		ZeroMemory(&wave, sizeof(WAVEOUTCAPS));
		if (waveOutGetDevCaps(i, &wave, sizeof(WAVEOUTCAPS)) != MMSYSERR_NOERROR) 
			return false;
		char ch[32];
		char defaultChar = ' ';
		WideCharToMultiByte(CP_ACP, 0, wave.szPname, -1, ch, 32, &defaultChar, NULL);

		vec.emplace_back(ch);
	}
	return true;
}

void SoundPlayer::GetAllCompatibleSounds(std::vector<std::string>& vec) const
{
	vec.clear();
	vec = toad::get_all_files_ext(toad::misc::exe_path, ".wav", true);

	auto rawfiles = toad::get_all_files_ext(toad::misc::exe_path, ".raw", true);
	if (!rawfiles.empty())
		vec.insert(vec.end(), rawfiles.begin(), rawfiles.end());
}

SoundBuffer SoundPlayer::LoadAudioBlockNew(fs::path sound_file)
{
	HANDLE hFile = NULL;
	DWORD readBytes = 0;
	std::string s;
	SoundBuffer buffer;

	if (sound_file.string().find(toad::misc::exe_path.string()) == std::string::npos)
	{
		sound_file = toad::misc::exe_path / sound_file;
	}

	if (sound_file.extension() == ".wav")
	{
		// get rid of the 44 bytes header,
		// #TODO: some .wav files have extra meta information. 

		if ((hFile = CreateFileA(sound_file.string().c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL)) == INVALID_HANDLE_VALUE)
		{
			LOG_ERRORF("[SoundPlayer] CreateFile Error: %ld", GetLastError());
			return buffer;
		}

		do {
			if ((buffer.size = GetFileSize(hFile, NULL) - 44) == 0)
				break;
			if ((buffer.buf = malloc(buffer.size)) == NULL)
				break;

			if (SetFilePointer(hFile, 44, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
			{
				LOG_ERRORF("[SoundPlayer] SetFilePointer Error: %ld", GetLastError());
				break;
			}
			if (ReadFile(hFile, buffer.buf, buffer.size, &readBytes, NULL) == FALSE)
			{
				free(buffer.buf);
				buffer.size = 0;
				break;
			}
		} while (false);

		CloseHandle(hFile);
		return buffer;
	}

	if ((hFile = CreateFileA(sound_file.string().c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL)) == INVALID_HANDLE_VALUE)
	{
		LOG_ERRORF("[SoundPlayer] CreateFile Error: %ld", GetLastError());
		return buffer;
	}

	// this will allocate a whole block of memory for the file that gets written to the wave device.

	do {
		if ((buffer.size = GetFileSize(hFile, NULL)) == 0)
			break;
		if ((buffer.buf = malloc(buffer.size)) == NULL)
			break;
		if (ReadFile(hFile, buffer.buf, buffer.size, &readBytes, NULL) == FALSE)
			break;
	} while (false);

	CloseHandle(hFile);

	return buffer;
}

void SoundPlayer::WriteAudioBlock()
{
	ZeroMemory(&m_header, sizeof(WAVEHDR));
	m_header.dwBufferLength = m_currentSoundBuf->size;
	m_header.lpData = (LPSTR)m_currentSoundBuf->buf;
	
	waveOutPrepareHeader(m_hWaveOut, &m_header, sizeof(WAVEHDR));
	waveOutWrite(m_hWaveOut, &m_header, sizeof(WAVEHDR));
}

void SoundPlayer::Thread()
{
	while (m_threadFlag)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cv.wait(lock, [&] { return !m_threadFlag || (m_play && !toad::clicksounds::selected_clicksounds.empty()); });

		if (!m_threadFlag)
			break;

		if (toad::clicksounds::randomize_volume)
			m_vol = 0xFFFF * toad::random_int(toad::clicksounds::volume_min, toad::clicksounds::volume_max) / 100;
		else 
			m_vol = 0xFFFF * toad::clicksounds::volume_percent / 100;
			
		PlaySound();

		m_play = false;
	}
}

void SoundPlayer::Reset()
{
	waveOutReset(m_hWaveOut);
	//waveOutClose(m_hWaveOut);
}

bool SoundPlayer::PlaySound()
{
	if (m_resetWaveOut) 
		Reset();

	if (!m_currentSoundBuf || !m_currentSoundBuf->buf)
		return false;
	
	waveOutSetVolume(m_hWaveOut, MAKELONG(m_vol, m_vol));

	//if (!LoadAudioBlockNew())
	//	return false;

	WriteAudioBlock();

	m_currentSoundBuf = nullptr;
	m_resetWaveOut = true;

	return true;
}
