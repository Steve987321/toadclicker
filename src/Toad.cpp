#include "pch.h"
#include "Toad.h"

namespace toad
{
	
void launch_threads() {
#ifndef _DEBUG
	std::thread(&MouseHook::Thread, p_mouseHook.get()).detach();
#endif // !_DEBUG
	
//    std::thread(&c_clicker::thread, p_clicker.get()).detach();
//    std::thread(&c_right_clicker::thread, p_right_clicker.get()).detach();
	
	std::thread(misc::window_scanner).detach();
}
	
bool init_toad()
{
	LOG_DEBUG("[toadclicker Init]");
	
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;

	WCHAR buf[MAX_PATH];
	GetModuleFileName(NULL, buf, MAX_PATH);
	std::wstring p(buf);
	misc::exe_path = converter.to_bytes(p);
	int j = 0;
	for (int i = misc::exe_path.length(); i > 0; i--)
	{
	    j++;
	    if (misc::exe_path[i] == '\\') {
	        misc::exe_path.erase(misc::exe_path.length() - j + 1);
	        break;
	    }
	}
	
	LOG_DEBUG(misc::exe_path);
	
	if (!p_SoundPlayer->GetAllOutputDevices(clicksounds::audio_device_list))
	{
	    LOG_ERROR("[toadclicker Init] Failed to retrieve audio output devices");
	    return false;
	}
	
	LOG_DEBUG("[toadclicker Init] Getting options.txt");
	
	const char* optionsPath = getenv("APPDATA");
	
	std::string pathstr = std::string(optionsPath);
	pathstr.append("\\.minecraft\\options.txt");
	
	std::vector<std::string> mc_options;
	
	std::ifstream f;
	f.open(pathstr, std::ios::in);
	if (f.is_open()) {
	    LOG_DEBUG("[toadclicker Init] Found minecraft options.txt");
	    options_found = true;
	    std::string line;
	    while (std::getline(f, line)) {
	        mc_options.push_back(line);
	    }
	    f.close();
	}
	else
	{
	    LOG_ERROR("[toadclicker Init] Failed to retrieve minecraft options.txt");
	    options_found = false;
	    goto LABLE_THREADLAUNCH;
	}
	
	LOG_DEBUG("[toadclicker Init] Retrieving hotbar settings");
	//remove options except for hotbar settings
	    
	int index = 0;
	for (unsigned int i = 0; i < mc_options.size(); i++)
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
	    LOG_ERROR("couldn't find hotbar settings");
	    options_found = false;
	    goto LABLE_THREADLAUNCH;
	}
	    
	mc_options.erase(mc_options.begin() + index + 9, mc_options.end());
	mc_options.erase(mc_options.begin(), mc_options.begin() + index);
	
	for (auto& mc_option : mc_options)
		for (int i = 0; i < mc_option.size(); i++)
	        if (mc_option[i] == ':')
	        {
	            mc_option.erase(0, i + 1);
	        }
	
	LOG_DEBUG("[toadclicker Init] Mapping hotkeys");
	
	// map LWJGL keys to virtual keycodes
	hotbar_virtual_keycodes = map_hotkeys(mc_options);
	
	LOG_DEBUG("[toadclicker Init] Mapped hotkeys");
	
	p_SoundPlayer->GetAllCompatibleSounds(clicksounds::sounds_list, clicksounds::selected_clicksounds);
	
	
LABLE_THREADLAUNCH:
	LOG_DEBUG("launching threads");
	launch_threads();
	
	is_running = true;
	
	return true;
}

}
