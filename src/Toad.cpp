#include "pch.h"
#include "Toad.h"

namespace toad
{
	
void launch_threads() {
#ifndef _DEBUG
	std::thread(&MouseHook::Thread, p_mouseHook.get()).detach();
#endif // !_DEBUG

	std::thread(misc::window_scanner).detach();
}
	
bool init_toad()
{
	LOG_DEBUG("[Init] Start");
	
	misc::exe_path = get_exe_path().parent_path().string();

	LOG_DEBUGF("[Init] File path: %s", misc::exe_path.c_str());
	
	if (!p_SoundPlayer->GetAllOutputDevices(clicksounds::audio_device_list))
	{
	    LOG_ERROR("[Init] Failed to retrieve audio output devices");
	    return false;
	}

	std::vector<std::string> available_config_files = config::get_all_toad_configs(misc::exe_path);

	if (!available_config_files.empty())
	{
		if (available_config_files.size() == 1)
			config::load_config(available_config_files[0]);
		else
		{
			// #TODO: get last edited one 
		}
	}

	LOG_DEBUG("[Init] Getting options.txt");
		
	std::string options_file_path = getenv("APPDATA");
	options_file_path.append("\\.minecraft\\options.txt");

	std::vector<std::string> mc_options;
	
	LOG_DEBUG("[Init] Trying to open: %s", options_file_path);

	std::ifstream f;
	f.open(options_file_path, std::ios::in);
	if (f.is_open()) 
	{
	    LOG_DEBUG("[Init] Reading minecraft options.txt");
	    options_found = true;
	    std::string line;
	    while (std::getline(f, line)) {
	        mc_options.push_back(line);
	    }
	    f.close();
	}
	else
	{
	    LOG_ERROR("[Init] Failed to retrieve minecraft options.txt");
	    options_found = false;
	    goto LABLE_THREADLAUNCH;
	}
	
	LOG_DEBUG("[Init] Retrieving hotbar settings from options.txt");

	size_t index = 0;
	for (size_t i = 0; i < mc_options.size(); i++)
	{
		LOG_DEBUG(mc_options[i]);
	    if (mc_options[i].find("key_key.hotbar") != std::string::npos)
	    {
	        index = i;
	        LOG_OK(index);
	        break;
	    }
	}
	if (index == 0)
	{
	    LOG_ERROR("[Init] Couldn't find hotbar settings");
	    options_found = false;
	    goto LABLE_THREADLAUNCH;
	}
	
	mc_options.erase(mc_options.begin() + index + 9, mc_options.end());
	mc_options.erase(mc_options.begin(), mc_options.begin() + index);
	
	for (auto& mc_option : mc_options)
		for (size_t i = 0; i < mc_option.size(); i++)
	        if (mc_option[i] == ':')
	        {
	            mc_option.erase(0, i + 1);
	        }
	
	LOG_DEBUG("[Init] Mapping hotkeys");
	
	// map LWJGL keys to virtual keycodes
	if (!map_hotkeys(mc_options, hotbar_virtual_keycodes))
		LOG_ERROR("[Init] Failed to map hotkeys");
	else
		LOG_DEBUG("[Init] Mapped hotkeys");
	
	p_SoundPlayer->GetAllCompatibleSounds(clicksounds::sounds_list, clicksounds::selected_clicksounds);
	
LABLE_THREADLAUNCH:
	LOG_DEBUG("[Init] Launching main threads");
	launch_threads();
	
	is_running = true;
	
	return true;
}

}
