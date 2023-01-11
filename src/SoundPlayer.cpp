#include "pch.h"
#include "SoundPlayer.h"
#include "toad.h"

bool c_SoundPlayer::loadAudioBlockNew()
{
	HANDLE hFile = NULL;
	DWORD readBytes = 0;
	std::string s;

	// needs the full path
	if (toad::clicksounds::selectedClicksounds.size() > 1)
		s = std::string(toad::misc::exePath + '\\' + toad::clicksounds::selectedClicksounds[toad::random_int(0, toad::clicksounds::selectedClicksounds.size() - 1)]);
	else
		s = std::string(toad::misc::exePath + '\\' + toad::clicksounds::selectedClicksounds[0]);

	if ((hFile = CreateFileA(s.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL)) == INVALID_HANDLE_VALUE)
	{
		log_error(GetLastError());
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

void c_SoundPlayer::writeAudioBlock()
{
	ZeroMemory(&m_header, sizeof(WAVEHDR));
	m_header.dwBufferLength = m_size;
	m_header.lpData = (LPSTR)m_block;
	
	waveOutPrepareHeader(m_hWaveOut, &m_header, sizeof(WAVEHDR));
	waveOutWrite(m_hWaveOut, &m_header, sizeof(WAVEHDR));

	m_onceFlag = true;
}

void c_SoundPlayer::thread()
{
	while (m_threadFlag)
	{
		if (toad::clickrecorder::enabled || toad::clicker::enabled)
		{
			if (toad::clicksounds::play && !toad::clicksounds::selectedClicksounds.empty())
			{
				log_debug(toad::clicksounds::volumePercent);
				if (toad::clicksounds::randomizeVol) m_vol = (0xFFFF * toad::random_int(toad::clicksounds::volmin, toad::clicksounds::volmax)) / 100;
				else m_vol = ((0xFFFF * toad::clicksounds::volumePercent) / 100);
			
			//	std::unique_lock<std::mutex> lock(m_mutex);
				this->play_sound();
				toad::clicksounds::play = false;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		else std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

void c_SoundPlayer::reset()
{
	free(m_block);
	m_block = nullptr;
	waveOutReset(m_hWaveOut);
	waveOutClose(m_hWaveOut);
}

// 

c_SoundPlayer::c_SoundPlayer()
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

bool c_SoundPlayer::play_sound()
{
	if (toad::clicksounds::selectedDevice.compare("none") == 0 || !toad::clicksounds::use_customOutput)
	{
		if (toad::clicksounds::selectedClicksounds.size() > 1)
			PlaySoundA(toad::clicksounds::selectedClicksounds[toad::random_int(0, toad::clicksounds::selectedClicksounds.size() - 1)].c_str(), NULL, SND_FILENAME | SND_NODEFAULT | SND_ASYNC);
		else 
			PlaySoundA(toad::clicksounds::selectedClicksounds[0].c_str(), NULL, SND_FILENAME | SND_NODEFAULT | SND_ASYNC);
		return true;
	}
	if (m_onceFlag) reset();

	if (waveOutOpen(&m_hWaveOut, toad::clicksounds::selectedDeviceID, &m_format, NULL, NULL, CALLBACK_NULL) != MMSYSERR_NOERROR)
		return false;
	

	//waveOutSetVolume(m_hWaveOut, MAKELONG(m_vol, m_vol));
	waveOutSetVolume(m_hWaveOut, MAKELONG(m_vol, m_vol));

	if (!loadAudioBlockNew())
		return false;
	writeAudioBlock();
	return true;
}

bool c_SoundPlayer::get_all_outputDevices(std::vector<std::string>& vec)
{
	vec.clear();
	int n = waveOutGetNumDevs();
	for (int i = 0; i < n; i++)
	{
		WAVEOUTCAPS wave;
		ZeroMemory(&wave, sizeof(WAVEOUTCAPS));
		if (waveOutGetDevCaps(i, &wave, sizeof(WAVEOUTCAPS)) != MMSYSERR_NOERROR) return false;
		char ch[32];
		char defaultChar = ' ';
		WideCharToMultiByte(CP_ACP, 0, wave.szPname, -1, ch, 32, &defaultChar, NULL);
		
		vec.emplace_back(std::string(ch));
	}
	return true;
}

bool c_SoundPlayer::get_audioDevVol(int* val)
{
	DWORD VOL = 0;
	MMRESULT res;
	if ((res = waveOutOpen(&m_hWaveOut, toad::clicksounds::selectedDeviceID, &m_format, NULL, NULL, CALLBACK_NULL)) != MMSYSERR_NOERROR)
	{
		log_error("failed to open device");
		log_error(res);
		return false;
	}

	if ((res = waveOutGetVolume(m_hWaveOut, &VOL)) != MMSYSERR_NOERROR)
	{
		log_error("failed to get device volume");
		log_error(res);
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
void c_SoundPlayer::get_all_compatible_sounds(std::vector<std::string>& vec, const std::vector<std::string>& vec_check) const
{
	vec.clear();
	vec = toad::getAllFilesExt(toad::misc::exePath, ".wav", true);

	auto rawfiles = toad::getAllFilesExt(toad::misc::exePath, ".raw", true);
	if (!rawfiles.empty())
		vec.insert(vec.end(), rawfiles.begin(), rawfiles.end());

	// don't include files that are already in vec_check
	
	if (!vec_check.empty())
		for (unsigned int i = 0; i < vec.size(); i++)
			for (unsigned int j = 0; j < vec_check.size(); j++)
				if (vec[i] == vec_check[j])
					vec.erase(vec.begin() + i);
}
