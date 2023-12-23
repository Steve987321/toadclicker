#include "pch.h"
#include "ClickRecorder.h"
#include "toad.h"

#include "app/Application.h"

namespace fs = std::filesystem;
namespace vars = toad::clickrecorder;

auto start_clock = std::chrono::high_resolution_clock::now();

void ClickRecorder::calc_vars()
{
	float fulltime = 0;
	for (auto f : vars::click_delays)
	{
		fulltime += f / vars::multiplier;
	}

	vars::total_clicks = vars::click_delays.size() / 2;
	vars::average_cps = (vars::total_clicks / fulltime) * 1000;
}

void ClickRecorder::start_playback_thread()
{
	m_is_playback = true;
	m_playback_thread = std::thread(&ClickRecorder::playback_thread, this);
}

void ClickRecorder::start_record_thread()
{
	m_is_recording = true;
	m_record_thread = std::thread(&ClickRecorder::record_thread, this);
}

void ClickRecorder::stop_playback_thread()
{
	m_is_playback = false;
	if (m_playback_thread.joinable())
		m_playback_thread.join();
}

void ClickRecorder::stop_record_thread()
{
	m_is_recording = false;
	can_save = false;
	if (m_record_thread.joinable())
		m_record_thread.join();
}

ClickRecorder& ClickRecorder::get()
{
	static ClickRecorder instance;
	return instance;
}

void ClickRecorder::reset()
{
	this->is_first_click = 0;
	this->mouse_press_counter = 0;
	vars::click_delays.clear();
	vars::fulltime = 0;
	vars::average_cps = 0.0;
}

void ClickRecorder::record_thread()
{
	while (m_is_recording)
	{
		if (vars::enabled)
		{
			elapsed = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(std::chrono::high_resolution_clock::now() - start_clock);

			if (!vars::enabled)
			{
				vars::record_status = RECORDSTATUS::NOT_RECORDING; 
				can_save = false;
			}
			else
				can_save = true;

			if (vars::record_status != RECORDSTATUS::AWAITING_FOR_CLICK && can_save && vars::skip_on_delay && elapsed.count() > vars::skip_delay_threshold * 1000) 
			{
				start_clock = std::chrono::high_resolution_clock::now();
				is_first_click = -1;
				vars::record_status = RECORDSTATUS::SKIPPING_NEXT_CLICK;
				can_save = false;
			}
		}
		else
		{
			vars::record_status = RECORDSTATUS::NOT_RECORDING; 
			can_save = false;
			std::this_thread::sleep_for(std::chrono::milliseconds(std::chrono::milliseconds(100)));
		}
	}
	
}

void ClickRecorder::save_delay()
{
	if (!can_save)
		return;

	if (is_first_click <= 0) {
		start_clock = std::chrono::high_resolution_clock::now();
		is_first_click++;
	}
	else
	{
		elapsed = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(std::chrono::high_resolution_clock::now() - start_clock);
		vars::click_delays.emplace_back(elapsed.count());

		vars::record_status = RECORDSTATUS::RECORDING;

		vars::fulltime += elapsed.count();
		start_clock = std::chrono::high_resolution_clock::now();

		mouse_press_counter++;
		if (mouse_press_counter % 2 == 0)
			vars::total_clicks++;
		vars::average_cps = (vars::total_clicks / vars::fulltime) * 1000;
	}
}

bool ClickRecorder::load_file(const std::string name, const std::string ext)
{
	vars::click_delays.clear();

	std::ifstream f;
	f.open(toad::misc::exe_path + "\\" + name + ext, std::ios::in);

	if (!f.is_open()) {
		LOGERROR("failed to open file");
		return false;
	}

	float num;
	while (f >> num) {
		vars::click_delays.emplace_back(num);
	}

	f.close();

	LOGDEBUG("File loaded");

	//calculate things again after successful load
	this->calc_vars();

	return true;
}

void ClickRecorder::save_file(const std::string name, const std::string ext)
{
	std::ofstream o;
	o.open(name + ext, std::ios_base::out);

	for (float click_delay : vars::click_delays)
		o << click_delay << std::endl;

	o.close();
}

void ClickRecorder::playback_thread()
{
	while (m_is_playback)
	{
		if (vars::playback_enabled)
		{
			if (GetAsyncKeyState(VK_LBUTTON))
			{
				for (unsigned int i = vars::click_start_point, j = vars::click_start_point + 1; j < vars::click_delays.size(); i++, j++)
				{
					if (!vars::playback_enabled) { vars::click_start_point = i % 2 == 0 ? i : i + 1; break; }
					if (!GetAsyncKeyState(VK_LBUTTON)) { 
						vars::randomize_start_point ? vars::click_start_point = toad::random_int(0, vars::click_delays.size())
							: vars::click_start_point = i;
						if (vars::click_start_point % 2 != 0) vars::click_start_point++;
						break; 
					}
					if (toad::clicker::cursor_visible && !vars::inventory) { vars::click_start_point = i % 2 == 0 ? i : i + 1; break; }

					int delay = toad::misc::compatibility_mode ? vars::click_delays[i] / vars::multiplier * 1000 - 1 : vars::click_delays[i] / vars::multiplier * 1000;
					int delay2 = toad::misc::compatibility_mode ? vars::click_delays[j] / vars::multiplier * 1000 - 1 : vars::click_delays[j] / vars::multiplier * 1000;

					//loop
					if (j == vars::click_delays.size() - 1) { i = 0; j = 1; }
					std::this_thread::sleep_for(std::chrono::microseconds(delay));

					if (toad::clicksounds::enabled) toad::clicksounds::play = true;

					PostMessage(toad::clicking_window, WM_LBUTTONDOWN, MKF_LEFTBUTTONDOWN, LPARAM((pt.x, pt.y)));

					std::this_thread::sleep_for(std::chrono::microseconds(delay2));

					PostMessage(toad::clicking_window, WM_LBUTTONUP, 0, LPARAM((pt.x, pt.y)));
				}
			}
			else 
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		else
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

bool vars::is_start_point_valid()
{
	return vars::click_start_point % 2 == 0;
}