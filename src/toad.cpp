#include "pch.h"
#include "toad.h"

#include "app/Application.h"


// main threads init
void toad::launch_threads() {
#ifndef _DEBUG
    std::thread(&c_mouseHook::thread, p_mouseHook.get()).detach();
#endif 

    std::thread(toad::misc::window_scanner).detach();
}

bool cursed_binds = false;
/// <summary>
/// Reads options.txt and converts the LWJGL keycodes to Windows keycodes.
/// </summary>
/// <param name="hotkeys">LWJGL hotkeys</param>
/// <returns>The windows keycodes in int types</returns>
std::vector<int> toad::map_hotkeys(std::vector<std::string>& hotkeys)
{
    //https://minecraft.fandom.com/el/wiki/Key_codes

    //first: LWGL, SECONDS: WIndows
    const std::unordered_map<int, int> mappedvalues{
        { 1, VK_ESCAPE },   // Escape
        { 2, 49 },  // 1
        { 3, 50 },  // 2
        { 4, 51 },  // 3
        { 5, 52 },  // 4
        { 6, 53 },  // 5
        { 7, 54 },  // 6
        { 8, 55 },  // 7
        { 9, 56 },  // 8
        { 10, 57 }, // 9
        { 11, 48 }, // 0
        { 12, 48 }, // -
        { 13, 48 }, // =
     // { 14, VK_BACK }, // backspace
     // { 15, VK_TAB  }, // tab
        { 16, 81 }, // Q
        { 17, 87 }, // W
        { 18, 69 }, // E
        { 19, 82 }, // R
        { 20, 84 }, // T
        { 21, 89 }, // Y
        { 22, 85 }, // U
        { 23, 73 }, // I
        { 24, 79 }, // O
        { 25, 80 }, // P
        { 30, 65 }, // A
        { 31, 83 }, // S
        { 32, 68 }, // D
        { 33, 70 }, // F
        { 34, 71 }, // G
        { 35, 72 }, // H
        { 36, 74 }, // J
        { 37, 75 }, // K 
        { 38, 76 }, // L
        { 44, 90 }, // Z
        { 45, 88 }, // X
        { 46, 67 }, // C
        { 47, 86 }, // V
        { 48, 66 }, // B
        { 49, 78 }, // N
        { 50, 77 }, // M
        { 58, 85 }, // caps lock

        //mouse  ?
    };

    std::vector<int> vec = {};

    for (int i = 0, j = 2; i < hotkeys.size(); i++, j++)
    {
        auto hotkey = hotkeys[i];
        int keycode = -1;

        try
        {
	        keycode = std::stoi(hotkey);
        }
        catch(const std::invalid_argument& e)
        {
            LOGERROR(e.what());
        }
        catch(const std::out_of_range& e)
        {
            LOGERROR(e.what());
        }

        auto it = mappedvalues.find(keycode == -1 ? j : keycode);
        if (it != mappedvalues.end())
            vec.push_back(it->second);
    }

    return vec;
}

bool toad::init_toad()
{
    LOGDEBUG("initializing");

    WCHAR buf[MAX_PATH];
    GetModuleFileName(NULL, buf, MAX_PATH);
    std::wstring p(buf);
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    toad::misc::exe_path = converter.to_bytes(p);

    for (int i = toad::misc::exe_path.length(), j = 0; i > 0; i--, j++)
    {
        if (toad::misc::exe_path[i] == '\\') {
            toad::misc::exe_path.erase(toad::misc::exe_path.length() - j + 1);
            break;
        }
    }

    const auto startThreads = [] {
        LOGDEBUG("initializing threads");
        toad::launch_threads();
        LOGDEBUG("threads initialized");

        toad::is_running = true;
    };

    if (!p_SoundPlayer->GetAllOutputDevices(toad::clicksounds::audio_device_list))
    {
        LOGERROR("failed to retrieve audio output devices");
        return false;
    }

    LOGDEBUG("getting options.txt");

    const char* optionsPath = getenv("APPDATA");

    std::string pathstr = std::string(optionsPath);
    pathstr.append("\\.minecraft\\options.txt");

    std::vector<std::string> mc_options;

    std::ifstream f;
    f.open(pathstr, std::ios::in);
    if (f.is_open()) {
        LOGDEBUG("found minecraft options.txt");
        toad::options_found = true;
        std::string buf;
        while (std::getline(f, buf)) {
            mc_options.push_back(buf);
        }
        f.close();
    }
    else
    {
        LOGERROR("failed to retrieve minecraft options.txt");
        toad::options_found = false;
        startThreads();
        return true;
    }

    LOGDEBUG("retrieving hotbar settings");
    //remove options except for hotbar settings
    
    int index = 0;
    for (unsigned int i = 0; i < mc_options.size(); i++)
    {
        //LOGDEBUG(mc_options[i]);
        if (mc_options[i].find("key_key.hotbar") != std::string::npos)
        {
            index = i;
            //log_ok(index);
            break;
        }
    }
    if (index == 0)
    {
        LOGERROR("couldn't find hotbar settings");
        toad::options_found = false;
        startThreads();
        return true;
    }
    
    mc_options.erase(mc_options.begin() + index + 9, mc_options.end());
    mc_options.erase(mc_options.begin(), mc_options.begin() + index);
    /*mc_options.erase(mc_options.end() - 16, mc_options.end());
    mc_options.erase(mc_options.begin(), mc_options.begin() + 78);*/

    for (auto& mc_option : mc_options)
	    for (int i = 0; i < mc_option.size(); i++)
            if (mc_option[i] == ':')
            {
                mc_option.erase(0, i + 1);
            }

    LOGDEBUG("mapping hotkeys");

    // map LWJGL keys to virtual keycodes
    toad::hotbar_vkcodes = toad::map_hotkeys(mc_options);

    LOGDEBUG("mapped hotkeys");

    p_SoundPlayer->GetAllCompatibleSounds(toad::clicksounds::sound_list, toad::clicksounds::selected_clicksounds);

    startThreads();

    return true;
}

bool toad::window_is_focused(const HWND& window)
{
    return GetForegroundWindow() == window;
}

std::vector<std::string> toad::get_all_files_ext(const std::filesystem::path& path, const char* ext, const bool includeExt)
{
    std::vector <std::string> vec = {};
    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        if (entry.path().extension() == std::move(ext))
            includeExt ? vec.push_back(entry.path().stem().string() + ext) : vec.push_back(entry.path().stem().string());
    }
    return vec;
}

