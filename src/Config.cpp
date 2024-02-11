#include "pch.h"
#include "toad.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

void toad::misc::load_config(const std::string configPath)
{
	std::ifstream f;
	f.open(configPath, std::ios::in);
	if (f.is_open())
	{
		json data = json::parse(f);

		toad::clicker::mincps = data["lcpsmin"];
		toad::clicker::maxcps = data["lcpsmax"];
		toad::clicker::cps = data["lcps"];
		toad::clicker::r::right_mincps = data["rcpsmin"];
		toad::clicker::r::right_mincps = data["rcpsmax"];

		//left options
		toad::clicker::enabled = data["lenabled"];
		toad::clicker::higher_cps = data["higher_cps"];
		toad::clicker::blatant_mode = data["blatant_mode"];
		toad::clicker::inventory = data["linventory"];
		toad::clicker::rmb_lock = data["rmb_lock"];
		toad::clicker::keycode = data["lkeycode"];
		toad::clicker::key = data["lkey"];
		toad::clicker::selected_enable_option = data["lenableoption"];
		toad::clicker::one_slider = data["lone_slider"];

		//double clicker
		toad::double_clicker::enabled = data["dclicker_enabled"];
		toad::double_clicker::delay = data["dclicker_delay"];
		toad::double_clicker::chance = data["dclicker_chance"];
		toad::double_clicker::min_interval = data["dmin_interval"];
		toad::double_clicker::max_interval = data["dmax_interval"];
		toad::double_clicker::keycode = data["dclicker_keycode"];
		toad::double_clicker::key = data["dclicker_key"];

		//slot whitelist
		toad::clicker::slot_whitelist = data["slot_whitelist"];
		toad::clicker::whitelisted_slots[0] = data["Slot0"];
		toad::clicker::whitelisted_slots[1] = data["Slot1"];
		toad::clicker::whitelisted_slots[2] = data["Slot2"];
		toad::clicker::whitelisted_slots[3] = data["Slot3"];
		toad::clicker::whitelisted_slots[4] = data["Slot4"];
		toad::clicker::whitelisted_slots[5] = data["Slot5"];
		toad::clicker::whitelisted_slots[6] = data["Slot6"];
		toad::clicker::whitelisted_slots[7] = data["Slot7"];
		toad::clicker::whitelisted_slots[8] = data["Slot8"];

		//Right
		toad::clicker::r::right_enabled = data["renabled"];
		toad::clicker::r::right_keycode = data["rkeycode"];
		toad::clicker::r::right_key = data["rkey"];
		toad::clicker::r::right_mincps = data["rmincps"];
		toad::clicker::r::right_maxcps = data["rmaxcps"];
		toad::clicker::r::right_inventory = data["rinventory"];
		toad::clicker::r::right_only_inventory = data["ronlyinventory"];
		toad::clicker::r::right_selected_enable_option = data["renableoption"];

		//Misc
		toad::misc::beep_on_toggle = data["beep_on_toggle"];
		toad::misc::compatibility_mode = data["compatibility_mode"];
		toad::misc::hide_key = data["hide_key"];
		toad::misc::selected_click_window = data["selected_click_window"];
		toad::misc::compatibility_mode = data["compatibility_mode"];
		toad::theme::main_col[0] = data["main_colr"];
		toad::theme::main_col[1] = data["main_colg"];
		toad::theme::main_col[2] = data["main_colb"];

		//jitter
		toad::jitter::enable = data["jenabled"];
		toad::jitter::intensity_Y = data["jyintensity"];
		toad::jitter::intensity_X = data["jxintensity"];
		toad::jitter::chance = data["jchance"];
		f.close();

		if (toad::clicker::enabled && !p_clicker->IsThreadAlive())
			p_clicker->StartThread();
		else if (!toad::clicker::enabled && p_clicker->IsThreadAlive())
			p_right_clicker->StopThread();

		if (toad::double_clicker::enabled && p_doubleClicker->IsThreadAlive())
			p_doubleClicker->StartThread();
		else if (!toad::clicker::r::right_enabled && p_doubleClicker->IsThreadAlive())
			p_doubleClicker->StopThread();

		if (toad::clicker::r::right_enabled && !p_right_clicker->IsThreadAlive())
			p_right_clicker->StartThread();
		else if (!toad::clicker::r::right_enabled && p_right_clicker->IsThreadAlive())
			p_right_clicker->StopThread();

		if (toad::clicksounds::enabled && p_SoundPlayer->IsThreadAlive())
			p_SoundPlayer->StartThread();
		else if (toad::clicksounds::enabled && !p_SoundPlayer->IsThreadAlive())
			p_SoundPlayer->StopThread();

	}
}

//save config
void toad::misc::save_config(std::string name)
{
	json j;
	j["lcpsmin"] = toad::clicker::mincps;
	j["lcpsmax"] = toad::clicker::maxcps;
	j["lcps"] = toad::clicker::cps;
	j["rcpsmin"] = toad::clicker::r::right_mincps;
	j["rcpsmax"] = toad::clicker::r::right_maxcps;
	
	//left options
	j["lenabled"] = toad::clicker::enabled;
	j["higher_cps"] = toad::clicker::higher_cps;
	j["blatant_mode"] = toad::clicker::blatant_mode;
	j["rmb_lock"] = toad::clicker::rmb_lock;
	j["linventory"] = toad::clicker::inventory;
	j["lkeycode"] = toad::clicker::keycode;
	j["lkey"] = toad::clicker::key;
	j["lenableoption"] = toad::clicker::selected_enable_option;
	j["lone_slider"] = toad::clicker::one_slider;

	//double clicker
	j["dclicker_key"] = toad::double_clicker::key;
	j["dclicker_keycode"] = toad::double_clicker::keycode;
	j["dclicker_enabled"] = toad::double_clicker::enabled;
	j["dclicker_delay"] = toad::double_clicker::delay;
	j["dmin_interval"] = toad::double_clicker::min_interval;
	j["dmax_interval"] = toad::double_clicker::max_interval;
	j["dclicker_chance"] = toad::double_clicker::chance;

	//slot whitelist
	j["slot_whitelist"] = toad::clicker::slot_whitelist;
	j["Slot0"] = toad::clicker::whitelisted_slots[0];
	j["Slot1"] = toad::clicker::whitelisted_slots[1];
	j["Slot2"] = toad::clicker::whitelisted_slots[2];
	j["Slot3"] = toad::clicker::whitelisted_slots[3];
	j["Slot4"] = toad::clicker::whitelisted_slots[4];
	j["Slot5"] = toad::clicker::whitelisted_slots[5];
	j["Slot6"] = toad::clicker::whitelisted_slots[6];
	j["Slot7"] = toad::clicker::whitelisted_slots[7];
	j["Slot8"] = toad::clicker::whitelisted_slots[8];

	//Right
	j["renabled"] = toad::clicker::r::right_enabled;
	j["rkeycode"] = toad::clicker::r::right_keycode;
	j["rkey"] = toad::clicker::r::right_key;
	j["rmincps"] = toad::clicker::r::right_mincps;
	j["rmaxcps"] = toad::clicker::r::right_maxcps;
	j["rinventory"] = toad::clicker::r::right_inventory;
	j["ronlyinventory"] = toad::clicker::r::right_only_inventory;
	j["renableoption"] = toad::clicker::r::right_selected_enable_option;

	//misc
	j["beep_on_toggle"] = toad::misc::beep_on_toggle;
	j["compatibility_mode"] = toad::misc::compatibility_mode;
	j["hide_key"] = toad::misc::hide_key;
	j["selected_click_window"] = toad::misc::selected_click_window;
	j["compatibility_mode"] = toad::misc::compatibility_mode;
	j["main_colr"] = toad::theme::main_col[0];
	j["main_colg"] = toad::theme::main_col[1];
	j["main_colb"] = toad::theme::main_col[2];

	//jitter
	j["jenabled"] = toad::jitter::enable;
	j["jyintensity"] = toad::jitter::intensity_Y;
	j["jxintensity"] = toad::jitter::intensity_X;
	j["jchance"] = toad::jitter::chance;

	//file extension
	name.append(".toad");
	std::ofstream o(name, std::ios::trunc);
	o << j;
	o.close();
}

std::vector<std::string> toad::misc::get_all_toad_configs(fs::path path)
{
	return toad::get_all_files_ext(path, ".toad");
}