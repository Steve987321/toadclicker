#include "pch.h"
#include "SoundPlayer.h"
#include "toad.h"

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

SoundPlayer::~SoundPlayer() = default;

void SoundPlayer::StartThread()
{
	m_threadFlag = true;
	m_thread = std::thread{ &SoundPlayer::Thread, this };
}

void SoundPlayer::StopThread()
{
	m_threadFlag = false;
	m_thread.join();
}

bool SoundPlayer::IsThreadAlive() const
{
	return m_threadFlag;
}

bool SoundPlayer::GetAllOutputDevices(std::vector<std::string>& vec)
{
	vec.clear();
	for (uint32_t i = 0; i < waveOutGetNumDevs(); i++)
	{
		WAVEOUTCAPS wave;
		ZeroMemory(&wave, sizeof(WAVEOUTCAPS));
		if (waveOutGetDevCaps(i, &wave, sizeof(WAVEOUTCAPS)) != MMSYSERR_NOERROR) return false;
		char ch[32];
		char defaultChar = ' ';
		WideCharToMultiByte(CP_ACP, 0, wave.szPname, -1, ch, 32, &defaultChar, NULL);

		vec.emplace_back(ch);
	}
	return true;
}

bool SoundPlayer::GetAudioDeviceVolume(int* val)
{
	DWORD VOL = 0;
	MMRESULT res;
	if ((res = waveOutOpen(&m_hWaveOut, toad::clicksounds::selected_device_ID, &m_format, NULL, NULL, CALLBACK_NULL)) != MMSYSERR_NOERROR)
	{
		LOG_ERROR("failed to open device");
		LOG_ERROR(res);
		return false;
	}

	if ((res = waveOutGetVolume(m_hWaveOut, &VOL)) != MMSYSERR_NOERROR)
	{
		LOG_ERROR("failed to get device volume");
		LOG_ERROR(res);
		return false;
	}

	//WORD left_channel_volume = VOL & 0xffff;           // extract the low order word
	//WORD right_channel_volume = (VOL >> 16);  // extract the high order word

	//unsigned left_volume_percent = left_channel_volume / 0xffff;

	//log_debug(left_channel_volume);
	//log_debug(right_channel_volume);
	//log_debug(left_volume_percent);

	waveOutReset(m_hWaveOut);
	waveOutClose(m_hWaveOut);
	return true;
}

// get raw and wav files
void SoundPlayer::GetAllCompatibleSounds(std::vector<std::string>& vec, const std::vector<std::string>& vec_check) const
{
	vec.clear();
	vec = toad::get_all_files_ext(toad::misc::exe_path, ".wav", true);

	auto rawfiles = toad::get_all_files_ext(toad::misc::exe_path, ".raw", true);
	if (!rawfiles.empty())
		vec.insert(vec.end(), rawfiles.begin(), rawfiles.end());

	// don't include files that are already in vec_check

	if (!vec_check.empty())
		for (uint32_t i = 0; i < vec.size(); i++)
			for (const std::string& j : vec_check)
				if (vec[i] == j)
					vec.erase(vec.begin() + i);
}

bool SoundPlayer::LoadAudioBlockNew()
{
	HANDLE hFile = NULL;
	DWORD readBytes = 0;
	std::string s;

	// needs the full path
	if (toad::clicksounds::selected_clicksounds.size() > 1)
		s = std::string(toad::misc::exe_path + '\\' + toad::clicksounds::selected_clicksounds[toad::random_int(0, toad::clicksounds::selected_clicksounds.size() - 1)]);
	else
		s = std::string(toad::misc::exe_path + '\\' + toad::clicksounds::selected_clicksounds[0]);

	if ((hFile = CreateFileA(s.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL)) == INVALID_HANDLE_VALUE)
	{
		LOG_ERROR(GetLastError());
		return false;
	}

	// this will allocate a whole block of memory for the file that gets written to the wave device.

	do {
		if ((m_size = GetFileSize(hFile, NULL)) == 0)
			break;
		if ((m_block = malloc(m_size)) == NULL)
			break;
		if (ReadFile(hFile, m_block, m_size, &readBytes, NULL) == FALSE)
			break;
	} while (false);
	CloseHandle(hFile);
	return true;
}

void SoundPlayer::WriteAudioBlock()
{
	ZeroMemory(&m_header, sizeof(WAVEHDR));
	m_header.dwBufferLength = m_size;
	m_header.lpData = (LPSTR)m_block;
	
	waveOutPrepareHeader(m_hWaveOut, &m_header, sizeof(WAVEHDR));
	waveOutWrite(m_hWaveOut, &m_header, sizeof(WAVEHDR));

	m_onceFlag = true;
}

void SoundPlayer::Thread()
{
	while (m_threadFlag)
	{
		if (toad::clickrecorder::enabled || toad::clicker::enabled)
		{
			if (toad::clicksounds::play && !toad::clicksounds::selected_clicksounds.empty())
			{
				LOG_DEBUG(toad::clicksounds::volume_percent);
				if (toad::clicksounds::randomize_volume) m_vol = (0xFFFF * toad::random_int(toad::clicksounds::volume_min, toad::clicksounds::volume_max)) / 100;
				else m_vol = ((0xFFFF * toad::clicksounds::volume_percent) / 100);
			
			//	std::unique_lock<std::mutex> lock(m_mutex);
				this->PlaySound();
				toad::clicksounds::play = false;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		else std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

void SoundPlayer::Reset()
{
	free(m_block);
	m_block = nullptr;
	waveOutReset(m_hWaveOut);
	waveOutClose(m_hWaveOut);
}

bool SoundPlayer::PlaySound()
{
	if (toad::clicksounds::selected_device == "none" || !toad::clicksounds::use_custom_output)
	{
		if (toad::clicksounds::selected_clicksounds.size() > 1)
			PlaySoundA(toad::clicksounds::selected_clicksounds[toad::random_int(0, toad::clicksounds::selected_clicksounds.size() - 1)].c_str(), NULL, SND_FILENAME | SND_NODEFAULT | SND_ASYNC);
		else 
			PlaySoundA(toad::clicksounds::selected_clicksounds[0].c_str(), NULL, SND_FILENAME | SND_NODEFAULT | SND_ASYNC);

		return true;
	}

	if (m_onceFlag) 
		Reset();

	if (waveOutOpen(&m_hWaveOut, toad::clicksounds::selected_device_ID, &m_format, NULL, NULL, CALLBACK_NULL) != MMSYSERR_NOERROR)
		return false;
	
	//waveOutSetVolume(m_hWaveOut, MAKELONG(m_vol, m_vol));
	waveOutSetVolume(m_hWaveOut, MAKELONG(m_vol, m_vol));

	if (!LoadAudioBlockNew())
		return false;
	WriteAudioBlock();
	return true;
}
