#include "pch.h"
#include "SoundPlayer.h"
#include "toad.h"

#include "app/Application.h"


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
	if (m_block != nullptr)
		reset();
	
	if (!m_onceFlag)
		waveOutReset(m_hWaveOut);

	if (m_thread.joinable())
	{
		m_threadFlag = false;
		m_thread.join();
	}
}

bool SoundPlayer::loadAudioBlockNew()
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
		if ((m_block = malloc(m_size)) == nullptr)
			break;
		if (ReadFile(hFile, m_block, m_size, &readBytes, NULL) == FALSE)
			break;
	} while (false);
	CloseHandle(hFile);
	return true;
}

void SoundPlayer::writeAudioBlock()
{
	ZeroMemory(&m_header, sizeof(WAVEHDR));
	m_header.dwBufferLength = m_size;
	m_header.lpData = (LPSTR)m_block;
	
	waveOutPrepareHeader(m_hWaveOut, &m_header, sizeof(WAVEHDR));
	waveOutWrite(m_hWaveOut, &m_header, sizeof(WAVEHDR));

	m_onceFlag = true;
}

void SoundPlayer::thread()
{
	while (m_threadFlag)
	{
		if (toad::clicksounds::play && !toad::clicksounds::selectedClicksounds.empty())
		{
			if (toad::clicksounds::randomizeVol)
				m_vol = (0xFFFF * toad::random_int(toad::clicksounds::volmin, toad::clicksounds::volmax)) / 100;
			else 
				m_vol = ((0xFFFF * toad::clicksounds::volumePercent) / 100);
			
			play_sound();
			toad::clicksounds::play = false;

		}
	}
}

void SoundPlayer::reset()
{
	free(m_block);
	m_block = nullptr;
	waveOutReset(m_hWaveOut);
}

bool SoundPlayer::play_sound()
{
	auto begin = std::chrono::high_resolution_clock::now();

	if (toad::clicksounds::selectedDevice.compare("none") == 0 || !toad::clicksounds::use_customOutput)
	{
		if (toad::clicksounds::selectedClicksounds.size() > 1)
			PlaySoundA(toad::clicksounds::selectedClicksounds[toad::random_int(0, toad::clicksounds::selectedClicksounds.size() - 1)].c_str(), NULL, SND_FILENAME | SND_NODEFAULT | SND_ASYNC);
		else 
			PlaySoundA(toad::clicksounds::selectedClicksounds[0].c_str(), NULL, SND_FILENAME | SND_NODEFAULT | SND_ASYNC);

		auto end = std::chrono::high_resolution_clock::now();
		log_debug(std::chrono::duration_cast<std::chrono::milliseconds>(end - begin));

		return true;
	}

	if (m_onceFlag) 
		reset();	

	waveOutSetVolume(m_hWaveOut, MAKELONG(m_vol, m_vol));

	if (!loadAudioBlockNew())
		return false;
	writeAudioBlock();

	auto end = std::chrono::high_resolution_clock::now();
	log_debug(std::chrono::duration_cast<std::chrono::milliseconds>(end - begin));
	return true;
}

bool SoundPlayer::SetAudioDevice(int id)
{
	m_deviceId = id;
	return waveOutOpen(&m_hWaveOut, m_deviceId, &m_format, NULL, NULL, CALLBACK_NULL) != MMSYSERR_NOERROR;
}

bool SoundPlayer::GetAllOutputDevices(std::vector<std::string>& vec)
{
	vec.clear();
	int n = waveOutGetNumDevs();
	for (int i = 0; i < n; i++)
	{
		WAVEOUTCAPS wave;
		ZeroMemory(&wave, sizeof(WAVEOUTCAPS));
		if (waveOutGetDevCaps(i, &wave, sizeof(WAVEOUTCAPS)) != MMSYSERR_NOERROR)
			return false;

		char ch[32];
		char defaultChar = ' ';
		WideCharToMultiByte(CP_ACP, 0, wave.szPname, -1, ch, 32, &defaultChar, NULL);
		
		vec.emplace_back(std::string(ch));
	}
	return true;
}

// get raw and wav files
void SoundPlayer::GetAllCompatibleSounds(std::vector<std::string>& vec, const std::vector<std::string>& vec_check) const
{
	vec.clear();
	vec = toad::getAllFilesExt(toad::misc::exePath, ".wav", true);

	auto rawfiles = toad::getAllFilesExt(toad::misc::exePath, ".raw", true);
	if (!rawfiles.empty())
		vec.insert(vec.end(), rawfiles.begin(), rawfiles.end());

	// don't include files that are already in vec_check
	
	if (!vec_check.empty())
		for (int i = 0; i < vec.size(); i++)
			for (int j = 0; j < vec_check.size(); j++)
				if (vec[i] == vec_check[j])
					vec.erase(vec.begin() + i);
}
