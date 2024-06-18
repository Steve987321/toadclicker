#include "pch.h"
#include "Config.h"
#include "Toad.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace toad
{

void config::load_config(const std::string& configPath)
{
	std::ifstream f;
	json data;

	f.open(configPath, std::ios::in);
	if (f.is_open())
	{
		try
		{
			data = json::parse(f);
			f.close();
		}
		catch (const json::parse_error& e)
		{
			LOG_ERRORF("[config] JSON parse error at {}, {}, for file {}", e.byte, e.what(), configPath);
			f.close();
			return;
		}
	}

	clicker::mincps = data["lcpsmin"];
	clicker::maxcps = data["lcpsmax"];
	clicker::cps = data["lcps"];
	clicker::r::right_mincps = data["rcpsmin"];
	clicker::r::right_mincps = data["rcpsmax"];

	//left options
	clicker::enabled = data["lenabled"];
	clicker::higher_cps = data["higher_cps"];
	clicker::blatant_mode = data["blatant_mode"];
	clicker::inventory = data["linventory"];
	clicker::rmb_lock = data["rmb_lock"];
	clicker::keycode = data["lkeycode"];
	clicker::key = data["lkey"];
	clicker::selected_enable_option = data["lenableoption"];
	clicker::one_slider = data["lone_slider"];

	//double clicker
	double_clicker::enabled = data["dclicker_enabled"];
	double_clicker::delay = data["dclicker_delay"];
	double_clicker::chance = data["dclicker_chance"];
	double_clicker::min_interval = data["dmin_interval"];
	double_clicker::max_interval = data["dmax_interval"];
	double_clicker::keycode = data["dclicker_keycode"];
	double_clicker::key = data["dclicker_key"];

	//slot whitelist
	clicker::slot_whitelist = data["slot_whitelist"];
	clicker::whitelisted_slots[0] = data["Slot0"];
	clicker::whitelisted_slots[1] = data["Slot1"];
	clicker::whitelisted_slots[2] = data["Slot2"];
	clicker::whitelisted_slots[3] = data["Slot3"];
	clicker::whitelisted_slots[4] = data["Slot4"];
	clicker::whitelisted_slots[5] = data["Slot5"];
	clicker::whitelisted_slots[6] = data["Slot6"];
	clicker::whitelisted_slots[7] = data["Slot7"];
	clicker::whitelisted_slots[8] = data["Slot8"];

	//Right
	clicker::r::right_enabled = data["renabled"];
	clicker::r::right_keycode = data["rkeycode"];
	clicker::r::right_key = data["rkey"];
	clicker::r::right_mincps = data["rmincps"];
	clicker::r::right_maxcps = data["rmaxcps"];
	clicker::r::right_inventory = data["rinventory"];
	clicker::r::right_only_inventory = data["ronlyinventory"];
	clicker::r::right_selected_enable_option = data["renableoption"];

	//Misc
	misc::beep_on_toggle = data["beep_on_toggle"];
	misc::compatibility_mode = data["compatibility_mode"];
	misc::hide_key = data["hide_key"];
	misc::selected_click_window = data["selected_click_window"];
	misc::compatibility_mode = data["compatibility_mode"];
	theme::main_col[0] = data["main_colr"];
	theme::main_col[1] = data["main_colg"];
	theme::main_col[2] = data["main_colb"];

	//jitter
	jitter::enabled = data["jenabled"];
	jitter::intensity_Y = data["jyintensity"];
	jitter::intensity_X = data["jxintensity"];
	jitter::chance = data["jchance"];

	if (clicker::enabled && !p_clicker->IsThreadAlive())
		p_clicker->StartThread();
	else if (!clicker::enabled && p_clicker->IsThreadAlive())
		p_rightClicker->StopThread();

	if (double_clicker::enabled && p_doubleClicker->IsThreadAlive())
		p_doubleClicker->StartThread();
	else if (!clicker::r::right_enabled && p_doubleClicker->IsThreadAlive())
		p_doubleClicker->StopThread();

	if (clicker::r::right_enabled && !p_rightClicker->IsThreadAlive())
		p_rightClicker->StartThread();
	else if (!clicker::r::right_enabled && p_rightClicker->IsThreadAlive())
		p_rightClicker->StopThread();

	if (clicksounds::enabled && p_SoundPlayer->IsThreadAlive())
		p_SoundPlayer->StartThread();
	else if (!clicksounds::enabled && !p_SoundPlayer->IsThreadAlive())
		p_SoundPlayer->StopThread();

	if (jitter::enabled && p_Jitter->IsThreadAlive())
		p_Jitter->StartThread();
	else if (!jitter::enabled && !p_Jitter->IsThreadAlive())
		p_Jitter->StopThread();
}

//save config
void config::save_config(std::string name)
{
	json j;
	j["lcpsmin"] = clicker::mincps;
	j["lcpsmax"] = clicker::maxcps;
	j["lcps"] = clicker::cps;
	j["rcpsmin"] = clicker::r::right_mincps;
	j["rcpsmax"] = clicker::r::right_maxcps;
	
	//left options
	j["lenabled"] = clicker::enabled;
	j["higher_cps"] = clicker::higher_cps;
	j["blatant_mode"] = clicker::blatant_mode;
	j["rmb_lock"] = clicker::rmb_lock;
	j["linventory"] = clicker::inventory;
	j["lkeycode"] = clicker::keycode;
	j["lkey"] = clicker::key;
	j["lenableoption"] = clicker::selected_enable_option;
	j["lone_slider"] = clicker::one_slider;

	//double clicker
	j["dclicker_key"] = double_clicker::key;
	j["dclicker_keycode"] = double_clicker::keycode;
	j["dclicker_enabled"] = double_clicker::enabled;
	j["dclicker_delay"] = double_clicker::delay;
	j["dmin_interval"] = double_clicker::min_interval;
	j["dmax_interval"] = double_clicker::max_interval;
	j["dclicker_chance"] = double_clicker::chance;

	//slot whitelist
	j["slot_whitelist"] = clicker::slot_whitelist;
	j["Slot0"] = clicker::whitelisted_slots[0];
	j["Slot1"] = clicker::whitelisted_slots[1];
	j["Slot2"] = clicker::whitelisted_slots[2];
	j["Slot3"] = clicker::whitelisted_slots[3];
	j["Slot4"] = clicker::whitelisted_slots[4];
	j["Slot5"] = clicker::whitelisted_slots[5];
	j["Slot6"] = clicker::whitelisted_slots[6];
	j["Slot7"] = clicker::whitelisted_slots[7];
	j["Slot8"] = clicker::whitelisted_slots[8];

	//Right
	j["renabled"] = clicker::r::right_enabled;
	j["rkeycode"] = clicker::r::right_keycode;
	j["rkey"] = clicker::r::right_key;
	j["rmincps"] = clicker::r::right_mincps;
	j["rmaxcps"] = clicker::r::right_maxcps;
	j["rinventory"] = clicker::r::right_inventory;
	j["ronlyinventory"] = clicker::r::right_only_inventory;
	j["renableoption"] = clicker::r::right_selected_enable_option;

	//misc
	j["beep_on_toggle"] = misc::beep_on_toggle;
	j["compatibility_mode"] = misc::compatibility_mode;
	j["hide_key"] = misc::hide_key;
	j["selected_click_window"] = misc::selected_click_window;
	j["compatibility_mode"] = misc::compatibility_mode;
	j["main_colr"] = theme::main_col[0];
	j["main_colg"] = theme::main_col[1];
	j["main_colb"] = theme::main_col[2];

	//jitter
	j["jenabled"] = jitter::enabled;
	j["jyintensity"] = jitter::intensity_Y;
	j["jxintensity"] = jitter::intensity_X;
	j["jchance"] = jitter::chance;

	//file extension
	name.append(".toad");
	std::ofstream o(name, std::ios::trunc);
	o << j;
	o.close();
}

std::vector<std::string> config::get_all_toad_configs(const fs::path& path)
{
	return get_all_files_ext(path, ".toad");
}

}
