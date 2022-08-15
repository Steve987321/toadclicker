#include "pch.h"
#include "toad.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

void toad::misc::loadConfig(const Preset& preset)
{
	clicker::enabled = true;
	clicker::r::right_enabled = preset.rEnabled;

	clicker::mincps = preset.lcpsmin;
	clicker::maxcps = preset.lcpsmax;
	clicker::r::right_mincps = preset.rcpsmin;
	clicker::r::right_maxcps = preset.rcpsmax;

	clicker::rmb_lock = preset.llmbLock;
	clicker::inventory = preset.linventory;
	clicker::blatant_mode = preset.lblatant;
	clicker::higher_cps = preset.lhigherCPS;

	clicker::r::right_inventory = preset.rInventory;
	clicker::r::right_only_inventory = preset.rOnlyInventory;
}

void toad::misc::loadConfig(const std::string configPath)
{
	std::ifstream f;
	f.open(configPath, std::ios::in);
	if (f.is_open())
	{
		json data = json::parse(f);

		toad::clicker::mincps = data["lcpsmin"];
		toad::clicker::maxcps = data["lcpsmax"];
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
		toad::clicker::selectedEnableOption = data["lenableoption"];

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
		toad::clicker::r::right_selectedEnableOption = data["renableoption"];

		//Misc
		toad::misc::beep_on_toggle = data["beep_on_toggle"];
		toad::misc::hide_key = data["hide_key"];
		toad::misc::selectedClickWindow = data["selected_click_window"];
		toad::theme::main_col[0] = data["main_colr"];
		toad::theme::main_col[1] = data["main_colg"];
		toad::theme::main_col[2] = data["main_colb"];

		//jitter
		toad::jitter::enable = data["jenabled"];
		toad::jitter::intensity_Y = data["jyintensity"];
		toad::jitter::intensity_X = data["jxintensity"];
		toad::jitter::chance = data["jchance"];
		f.close();

	}
}
void toad::misc::createConfig(std::string name)
{
	json j;

	j["lcpsmin"] = toad::clicker::mincps;
	j["lcpsmax"] = toad::clicker::maxcps;
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
	j["lenableoption"] = toad::clicker::selectedEnableOption;

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
	j["renableoption"] = toad::clicker::r::right_selectedEnableOption;

	//misc
	j["beep_on_toggle"] = toad::misc::beep_on_toggle;
	j["hide_key"] = toad::misc::hide_key;
	j["selected_click_window"] = toad::misc::selectedClickWindow;
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
	std::ofstream o(name);
	o << j;
	o.close();
}

//save config
void toad::misc::saveConfig(std::string name)
{
	json j;
	j["lcpsmin"] = toad::clicker::mincps;
	j["lcpsmax"] = toad::clicker::maxcps;
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
	j["lenableoption"] = toad::clicker::selectedEnableOption;

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
	j["renableoption"] = toad::clicker::r::right_selectedEnableOption;

	//misc
	j["beep_on_toggle"] = toad::misc::beep_on_toggle;
	j["hide_key"] = toad::misc::hide_key;
	j["selected_click_window"] = toad::misc::selectedClickWindow;
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

std::vector<std::string> toad::misc::GetAllToadConfigs(fs::path path)
{
	std::vector <std::string> vec = {};
	for (const auto& entry : fs::directory_iterator(path))
	{
		if (entry.path().extension() == ".toad")
			vec.push_back(entry.path().stem().string());
	}
	return vec;
}