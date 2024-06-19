#include "pch.h"
#include "ClickRecorder.h"
#include "Toad.h"

namespace fs = std::filesystem;

auto start_clock = std::chrono::high_resolution_clock::now();

ClickRecorder::~ClickRecorder()
{
	StopPlaybackThread();
	StopRecordingThread();
}

void ClickRecorder::CalcVars()
{
	float fulltime = 0;
	for (float click_delay : toad::clickrecorder::click_delays)
		fulltime += click_delay / toad::clickrecorder::multiplier;

	toad::clickrecorder::total_clicks = toad::clickrecorder::click_delays.size() / 2;
	toad::clickrecorder::average_cps = (toad::clickrecorder::total_clicks / fulltime) * 1000;
}

void ClickRecorder::Reset()
{
	this->isFirst_click = 0;
	this->k = 0;
	toad::clickrecorder::click_delays.clear();
	toad::clickrecorder::fulltime = 0;
	toad::clickrecorder::average_cps = 0.0;
}

void ClickRecorder::VarsCheckThread()
{
	while (m_recordFlag)
	{
		std::unique_lock lock(m_recordMutex);
		m_recordCV.wait(lock, [&] {return !m_recordFlag || toad::clickrecorder::enabled; });

		if (!toad::clickrecorder::enabled)
		{
			toad::clickrecorder::record_status = toad::clickrecorder::recordStatus::NOT_RECORDING;
			can_save = false;
		}

		if (!m_recordFlag)
			break;

		elapsed = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(std::chrono::high_resolution_clock::now() - start_clock);

		if (!toad::clickrecorder::enabled) 
		{ 
			toad::clickrecorder::record_status = toad::clickrecorder::recordStatus::NOT_RECORDING;
			can_save = false; 
		}

		else
			can_save = true;

		if (toad::clickrecorder::record_status != toad::clickrecorder::recordStatus::AWAITING_FOR_CLICK && can_save && toad::clickrecorder::skip_on_delay && (float)elapsed.count() > toad::clickrecorder::skip_delay * 1000)
		{
			start_clock = std::chrono::high_resolution_clock::now();
			isFirst_click = -1;
			toad::clickrecorder::record_status = toad::clickrecorder::recordStatus::SKIPPING_NEXT_CLICK;
			can_save = false;
		}
	}
	
}

void ClickRecorder::SaveDelay()
{
	if (!can_save)
		return;

	if (isFirst_click <= 0) {
		start_clock = std::chrono::high_resolution_clock::now();
		isFirst_click++;
	}
	else
	{
		toad::clickrecorder::record_status = toad::clickrecorder::recordStatus::RECORDING;

		elapsed = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(std::chrono::high_resolution_clock::now() - start_clock);
		toad::clickrecorder::click_delays.emplace_back(float(elapsed.count()));
		toad::clickrecorder::fulltime += float(elapsed.count());
		start_clock = std::chrono::high_resolution_clock::now();

		k++;
		if (k % 2 == 0) 
			toad::clickrecorder::total_clicks++;
		toad::clickrecorder::average_cps = (toad::clickrecorder::total_clicks / toad::clickrecorder::fulltime) * 1000;
	}
}

bool ClickRecorder::LoadFile(const std::string& name, const std::string& ext)
{
	toad::clickrecorder::click_delays.clear();

	std::ifstream f;
	//custom_ext ? f.open(toad::misc::exePath + "\\" + name, std::ios::in) : f.open(toad::misc::exePath + "\\" + name + ext, std::ios::in);
	f.open(toad::misc::exe_path / (name + ext), std::ios::in);

	if (!f.is_open()) {
		LOG_ERROR("failed to open file");
		return false;
	}

	float num;
	while (f >> num) {
		toad::clickrecorder::click_delays.emplace_back(num);
	}

	f.close();

	LOG_DEBUG("File loaded");

	//calculate things again after successful load
	this->CalcVars();

	return true;
}

void ClickRecorder::SaveFile(const std::string& name, const std::string& ext)
{
	std::ofstream o;
	//custom_ext ? o.open(name, std::ios_base::out) : o.open(name + ".txt", std::ios_base::out);
	o.open(name + ext, std::ios_base::out);

	for (float click_delay : toad::clickrecorder::click_delays) o << click_delay << std::endl;

	o.close();
}

void ClickRecorder::StartPlaybackThread()
{
	m_playbackFlag = true;
	m_playbackThread = std::thread{ &ClickRecorder::PlaybackThread, this };
}

void ClickRecorder::StopPlaybackThread()
{
	{
		std::lock_guard lock(m_playbackMutex);
		m_playbackFlag = false;
	}

	m_playbackCV.notify_one();

	if (m_playbackThread.joinable())
		m_playbackThread.join();
}

bool ClickRecorder::IsPlaybackThreadAlive() const
{
	return m_playbackFlag;
}

void ClickRecorder::StartRecordingThread()
{
	m_recordFlag = true;
	m_recordThread = std::thread{ &ClickRecorder::VarsCheckThread, this };
}

void ClickRecorder::StopRecordingThread()
{
	{
		std::lock_guard lock(m_recordMutex);
		m_recordFlag = false;
	}

	m_recordCV.notify_one();

	if (m_recordThread.joinable())
		m_recordThread.join();
}

bool ClickRecorder::IsRecordingThreadAlive() const
{
	return m_recordFlag;
}

void ClickRecorder::PlaybackThread()
{
	while (m_playbackFlag)
	{
		std::unique_lock lock(m_playbackMutex);
		m_playbackCV.wait(lock, [&] { return !m_playbackFlag || toad::clickrecorder::playback_enabled; });

		if (!m_playbackFlag)
			break;

		if (GetAsyncKeyState(VK_LBUTTON))
		{
			for (uint32_t i = toad::clickrecorder::click_start_point, j = toad::clickrecorder::click_start_point + 1; j < toad::clickrecorder::click_delays.size(); i++, j++)
			{
				if (!toad::clickrecorder::playback_enabled)
				{
					toad::clickrecorder::click_start_point = i % 2 == 0 ? i : i + 1;
					break;
				}

				if (!GetAsyncKeyState(VK_LBUTTON))
				{
					if (toad::clickrecorder::randomize_start_point)
						toad::clickrecorder::click_start_point = toad::random_int(0, toad::clickrecorder::click_delays.size());
					else
						toad::clickrecorder::click_start_point = i;

					if (toad::clickrecorder::click_start_point % 2 != 0)
						toad::clickrecorder::click_start_point++;

					break;
				}

				if (toad::clicker::cursor_visible && !toad::clickrecorder::inventory)
				{
					toad::clickrecorder::click_start_point = i % 2 == 0 ? i : i + 1;
					break;
				}

				int delay = toad::misc::compatibility_mode ? toad::clickrecorder::click_delays[i] / toad::clickrecorder::multiplier * 1000 - 1 : toad::clickrecorder::click_delays[i] / toad::clickrecorder::multiplier * 1000;
				int delay2 = toad::misc::compatibility_mode ? toad::clickrecorder::click_delays[j] / toad::clickrecorder::multiplier * 1000 - 1 : toad::clickrecorder::click_delays[j] / toad::clickrecorder::multiplier * 1000;

				//loop
				if (j == toad::clickrecorder::click_delays.size() - 1)
				{
					i = 0;
					j = 1;
				}

				if (toad::misc::compatibility_mode)
				{
					toad::precise_sleep(delay / 1e6);
				}
				else
				{
					std::this_thread::sleep_for(std::chrono::microseconds(delay));
				}

				PostMessage(toad::clicking_window, WM_LBUTTONDOWN, MKF_LEFTBUTTONDOWN, LPARAM((pt.x, pt.y)));

				if (toad::clicksounds::enabled && !toad::clicksounds::selected_clicksounds.empty())
				{
					selected_clicksound = (size_t)toad::random_int(0, toad::clicksounds::selected_clicksounds.size() - 1);
					auto& sound = toad::clicksounds::selected_clicksounds[selected_clicksound];
					p_SoundPlayer->TriggerSoundPlay(sound.first.string());
				}

				if (toad::misc::compatibility_mode)
				{
					toad::precise_sleep(delay2 / 1e6);
				}
				else
				{
					std::this_thread::sleep_for(std::chrono::microseconds(delay2));
				}

				if (toad::clicksounds::enabled && !toad::clicksounds::selected_clicksounds.empty())
				{
					auto& sound = toad::clicksounds::selected_clicksounds[selected_clicksound];
					if (!sound.second.empty())
						p_SoundPlayer->TriggerSoundPlay(sound.second.string());
				}
				PostMessage(toad::clicking_window, WM_LBUTTONUP, 0, LPARAM((pt.x, pt.y)));
			}
		}
	}
}

bool toad::clickrecorder::is_start_point_valid()
{
	return toad::clickrecorder::click_start_point % 2 == 0;
}