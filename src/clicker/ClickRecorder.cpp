#include "pch.h"
#include "ClickRecorder.h"
#include "toad.h"

namespace fs = std::filesystem;

auto start_clock = std::chrono::steady_clock::now();

void c_clickRecorder::calcVars()
{
	toad::clickrecorder::edited_click_delays.clear();
	for (unsigned int i = 0; i < toad::clickrecorder::click_delays.size(); i++)
	{
		toad::clickrecorder::edited_click_delays.emplace_back(toad::clickrecorder::click_delays[i]);
		if (toad::clickrecorder::click_delays[i] / toad::clickrecorder::multiplier < 0) continue;
		toad::clickrecorder::edited_click_delays[i] /= toad::clickrecorder::multiplier;
	}
	std::cout << toad::clickrecorder::edited_click_delays[0] << std::endl;

	auto getfulltime = []() {
		float temp = 0.0f;
		for (size_t i = 0; i < toad::clickrecorder::edited_click_delays.size(); i++) temp += toad::clickrecorder::edited_click_delays[i];
		return temp;
	};

	auto minmax = std::minmax_element(toad::clickrecorder::edited_click_delays.begin(), toad::clickrecorder::edited_click_delays.end());
	const float fulltime = getfulltime();
	toad::clickrecorder::total_clicks = toad::clickrecorder::edited_click_delays.size() / 2;
	toad::clickrecorder::average_cps = (toad::clickrecorder::total_clicks / fulltime) * 1000;
}

void c_clickRecorder::init_playback_thread()
{
	log_debug("starting click playback thread");
	std::thread(&c_clickRecorder::playback_thread, p_clickRecorder.get()).detach();
	toad::clickplayback_thread_exists = true;
}
void c_clickRecorder::init_record_thread()
{
	log_debug("starting click recorder thread");
	std::thread(&c_clickRecorder::vars_check_thread, p_clickRecorder.get()).detach();
	toad::clickrecord_thread_exists = true;
}

void c_clickRecorder::reset()
{
	this->isFirst_click = 0;
	this->k = 0;
	toad::clickrecorder::edited_click_delays.clear();
	toad::clickrecorder::click_delays.clear();
	toad::clickrecorder::fulltime = 0;
	toad::clickrecorder::average_cps = 0.0;
}

void c_clickRecorder::vars_check_thread()
{
	while (toad::is_running)
	{
		if (toad::clickrecorder::enabled)
		{
			elapsed = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(std::chrono::steady_clock::now() - start_clock);

			if (!toad::clickrecorder::enabled) { toad::clickrecorder::record_status = toad::clickrecorder::recordStatus::NOT_RECORDING; can_save = false; }
			else can_save = true;
			if (toad::clickrecorder::record_status != toad::clickrecorder::recordStatus::AWAITING_FOR_CLICK && can_save && toad::clickrecorder::skip_on_delay && (float)elapsed.count() > toad::clickrecorder::skip_delay * 1000) 
			{
				start_clock = std::chrono::steady_clock::now();
				isFirst_click = -1;
				toad::clickrecorder::record_status = toad::clickrecorder::recordStatus::SKIPPING_NEXT_CLICK;
				can_save = false;
			}
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(std::chrono::milliseconds(100)));
		}
	}
	
}

void c_clickRecorder::save_delay()
{
	if (!can_save) return;

	if (isFirst_click <= 0) {
		start_clock = std::chrono::steady_clock::now();
		isFirst_click++;
	}
	else
	{
		toad::clickrecorder::record_status = toad::clickrecorder::recordStatus::RECORDING;

		elapsed = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(std::chrono::steady_clock::now() - start_clock);
		toad::clickrecorder::edited_click_delays.emplace_back(float(elapsed.count()));
		toad::clickrecorder::fulltime += float(elapsed.count());
		start_clock = std::chrono::steady_clock::now();

		k++;
		if (k % 2 == 0) toad::clickrecorder::total_clicks++;
		toad::clickrecorder::average_cps = (toad::clickrecorder::total_clicks / toad::clickrecorder::fulltime) * 1000;
	}
}

bool c_clickRecorder::load_file(const std::string name, const std::string ext)
{
	toad::clickrecorder::click_delays.clear();

	std::ifstream f;
	//custom_ext ? f.open(toad::misc::exePath + "\\" + name, std::ios::in) : f.open(toad::misc::exePath + "\\" + name + ext, std::ios::in);
	f.open(toad::misc::exePath + "\\" + name + ext, std::ios::in);

	float num;
	if (!f.is_open()) {
		log_error("failed to open file");
		return false;
	}

	while (f >> num) {
		toad::clickrecorder::click_delays.emplace_back(num);
	}

	f.close();

	log_debug("File loaded");

	//calculate things again after successful load
	this->calcVars();

	return true;
}

void c_clickRecorder::save_file(const std::string name, const std::string ext)
{
	std::ofstream o;
	//custom_ext ? o.open(name, std::ios_base::out) : o.open(name + ".txt", std::ios_base::out);
	o.open(name + ext, std::ios_base::out);

	for (int i = 0; i < toad::clickrecorder::click_delays.size(); i++) o << toad::clickrecorder::click_delays[i] << std::endl;

	o.close();
}

void c_clickRecorder::playback_thread()
{
	bool flag = false;
	while (toad::is_running)
	{
		if (toad::clickrecorder::playback_enabled)
		{
			if (GetAsyncKeyState(VK_LBUTTON))
			{
				flag = false;
				for (unsigned int i = toad::clickrecorder::click_start_point, j = toad::clickrecorder::click_start_point + 1; j < toad::clickrecorder::edited_click_delays.size(); i++, j++)
				{
					if (!toad::clickrecorder::playback_enabled) { toad::clickrecorder::click_start_point = i % 2 == 0 ? i : i + 1; break; }
					if (!GetAsyncKeyState(VK_LBUTTON)) { 
						toad::clickrecorder::randomize_start_point ? toad::clickrecorder::click_start_point = toad::random_int(0, toad::clickrecorder::edited_click_delays.size())
							: toad::clickrecorder::click_start_point = i;
						if (toad::clickrecorder::click_start_point % 2 != 0) toad::clickrecorder::click_start_point++;
						break; 
					}
					if (toad::clicker::cursor_visible && !toad::clickrecorder::inventory) { toad::clickrecorder::click_start_point = i % 2 == 0 ? i : i + 1; break; }

					//loop
					if (j == toad::clickrecorder::edited_click_delays.size() - 1) { i = 0; j = 1; }
					std::this_thread::sleep_for(std::chrono::microseconds((int(toad::clickrecorder::edited_click_delays[i] * 1000))));

					PostMessage(toad::clicking_window, WM_LBUTTONDOWN, MKF_LEFTBUTTONDOWN, LPARAM((pt.x, pt.y)));

					std::this_thread::sleep_for(std::chrono::microseconds((int(toad::clickrecorder::edited_click_delays[j] * 1000))));

					PostMessage(toad::clicking_window, WM_LBUTTONUP, 0, LPARAM((pt.x, pt.y)));

				}
			}
		}
		else
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

bool toad::clickrecorder::is_start_point_valid()
{
	return toad::clickrecorder::click_start_point % 2 == 0;
}