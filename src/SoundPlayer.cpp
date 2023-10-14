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
	for (const auto& [_, hdr] : m_cached)
	{
		free(hdr.lpData);
	}

	reset();
	
	waveOutReset(m_hWaveOut);

	if (m_thread.joinable())
	{
		m_threadFlag = false;
		m_thread.join();
	}
}

void SoundPlayer::start_thread()
{
	m_threadFlag = true;
	m_thread = std::thread{ &SoundPlayer::thread, this };
}

void SoundPlayer::stop_thread()
{
	m_threadFlag = false;
	m_thread.join();
}

bool SoundPlayer::is_thread_alive() const
{
	return m_threadFlag;
}

DWORD SoundPlayer::to_dword_multiplier(float multiplier)
{
	int32_t n = (int32_t)multiplier;
	float frac = multiplier - n;

	DWORD l = (DWORD)n << 16;
	DWORD h = (DWORD)(frac * 0x10000);

	return l | h;
}

void SoundPlayer::writeAudioBlock(WAVEHDR* data)
{	
	waveOutPrepareHeader(m_hWaveOut, data, sizeof(WAVEHDR));
	waveOutWrite(m_hWaveOut, data, sizeof(WAVEHDR));
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
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void SoundPlayer::reset()
{
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

	reset();	

	//waveOutSetVolume(m_hWaveOut, MAKELONG(m_vol, m_vol));

	auto it = m_cached.begin();
	std::advance(it, toad::random_int(0, m_cached.size() - 1));
	writeAudioBlock(&it->second);

	auto end = std::chrono::high_resolution_clock::now();
	log_debug(std::chrono::duration_cast<std::chrono::milliseconds>(end - begin));
	return true;
}

bool SoundPlayer::CacheAudioFiles(std::vector<std::string>& files)
{
	// clear previous 
	for (const auto& [_, hdr] : m_cached)
	{
		free(hdr.lpData);
	}

	m_cached.clear();

	for (const auto& file : files)
	{
		std::string s = toad::misc::exePath + std::string(file);
		if (!CacheAudioFile(s))
			return false;
	}
	return true;
}

bool SoundPlayer::CacheAudioFile(std::string_view file_path)
{
	HANDLE hFile = NULL;
	DWORD readBytes = 0;
	WAVEHDR hdr{};
	ZeroMemory(&hdr, sizeof(WAVEHDR));

	if ((hFile = CreateFileA(file_path.data(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL)) == INVALID_HANDLE_VALUE)
	{
		log_errorf("error while trying to cache file : %d", GetLastError());
		return false;
	}

	// this will allocate a whole block of memory for the file that gets written to the wave device.

	do {
		if ((hdr.dwBufferLength = GetFileSize(hFile, NULL)) == 0)
		{
			log_error("Failed caching audio file 1");
			break;
		}
		if ((hdr.lpData = (LPSTR)malloc(hdr.dwBufferLength)) == nullptr)
		{
			log_error("Failed caching audio file 2");
			break;
		}
		if (ReadFile(hFile, hdr.lpData, hdr.dwBufferLength, &readBytes, NULL) == FALSE)
		{
			log_error("Failed caching audio file 3");
			break;
		}
	} while (false);

	CloseHandle(hFile);

	m_cached.insert({file_path.data(), hdr });

	return true;
}

bool SoundPlayer::ClearCachedFile(std::string_view file_path)
{
	auto it = m_cached.find(file_path.data());
	if (it != m_cached.end())
	{
		m_cached.erase(it);
		return true;
	}
	return false;
}

bool SoundPlayer::SetAudioDevice(int id)
{
	m_deviceId = id;
	return waveOutOpen(&m_hWaveOut, m_deviceId, &m_format, NULL, NULL, CALLBACK_NULL) != MMSYSERR_NOERROR;
}

void SoundPlayer::SetPlayBackRate(float multiplier)
{
	auto res = waveOutSetPlaybackRate(m_hWaveOut, to_dword_multiplier(multiplier));
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
