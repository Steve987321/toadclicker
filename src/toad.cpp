#include "pch.h"
#include "toad.h"

std::string toad::keys[] = {
       "Left mouse",
       "Right mouse",
       "Control-break",
       "mmb",
       "X1 mouse",
       "X2 mouse",
       "unknown",
       "BACKSPACE",
       "TAB",
       "unknown",
       "unknown",
       "CLEAR",
       "ENTER",
       "unknown",
       "unknown",
       "SHIFT",
       "CTRL",
       "ALT",
       "PAUSE",
       "CAPS LOCK",
       "IME Kana",
       "unknown",
       "IME Junja",
       "IME final",
       "IME Hanja kanji",
       "unknown",
       "ESC",
       "IME",
       "IME",
       "IME",
       "IME mode change",
       "Space bar",
       "PAGE UP",
       "PAGE DOWN",
       "END",
       "HOME",
       "LEFT ARROW",
       "UP ARROW",
       "RIGHT ARROW",
       "DOWN ARROW",
       "SELECT",
       "PRINT",
       "EXECUTE",
       "PRINT SCREEN",
       "INS",
       "DEL",
       "HELP",
       "0",
       "1",
       "2",
       "3",
       "4",
       "5",
       "6",
       "7",
       "8",
       "9",
       "unknown",
       "unknown",
       "unknown",
       "unknown",
       "unknown",
       "unknown",
       "unknown",
       "A",
       "B",
       "C",
       "D",
       "E",
       "F",
       "G",
       "H",
       "I",
       "J",
       "K",
       "L",
       "M",
       "N",
       "O",
       "P",
       "Q",
       "R",
       "S",
       "T",
       "U",
       "V",
       "W",
       "X",
       "Y",
       "Z",
       "Left Windows",
       "Right Windows",
       "Applications",
       "Reserved",
       "Computer Sleep",
       "Numeric keypad 0",
       "Numeric keypad 1",
       "Numeric keypad 2",
       "Numeric keypad 3",
       "Numeric keypad 4",
       "Numeric keypad 5",
       "Numeric keypad 6",
       "Numeric keypad 7",
       "Numeric keypad 8",
       "Numeric keypad 9",
       "Multiply",
       "ADD",
       "Separator",
       "Subtract",
       "Decimal",
       "Divide",
       "F1",
       "F2",
       "F3",
       "F4",
       "F5",
       "F6",
       "F7",
       "F8",
       "F9",
       "F10",
       "F11",
       "F12",
       "F13",
       "F14",
       "F15",
       "F16",
       "F17",
       "F18",
       "F19",
       "F20",
       "F21",
       "F22",
       "F23",
       "F24",
       "unassigned",
       "unassigned",
       "unassigned",
       "unassigned",
       "unassigned",
       "unassigned",
       "unassigned",
       "unassigned",
       "NUM LOCK",
       "SCROLL LOCK",
       "OEM specific",
       "OEM specific",
       "OEM specific",
       "OEM specific",
       "OEM specific",
       "unassigned",
       "unassigned",
       "unassigned",
       "unassigned",
       "unassigned",
       "unassigned",
       "unassigned",
       "unassigned",
       "unassigned",
       "LSHIFT",
       "RSHIFT",
       "LCONTROL", 
       "RCONTROL",
       "LMENU",
       "RMENU",
       "Back ",
       "Forward ",
       "Refresh ",
       "Stop ",
       "Search ",
       "Favorites ",
       "Start",
        "Volume Down" ,
        "Volume Mute" ,
        "Volume Up" ,
        "Next Track" ,
        "Previous Track" ,
        "Stop Media" ,
        "Play/Pause" ,
        "Start Mail" ,
        "Select Media" ,
        "Application 1" ,
        "Application 2" ,
        "reserved",
        "reserved",
        ";" ,
        "=",
        ",",
        "-",
        "'",
        "/" ,
        "`" ,
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Unassigned",
        "Unassigned",
        "Unassigned",
        "Unassigned",
        "[",
        "\\",
        "]",
        "'" ,
         "OEM8",
         "Reserved",
          "OEM specific",
           "\\", 
           "OEM specific",
           "OEM specific",
           "PROCESS",
           "OEM specific",
           "packet", 
           "Unassigned",
           "OEM specific",
           "OEM specific",
           "OEM specific",
           "OEM specific",
           "OEM specific",
           "OEM specific",
           "OEM specific",
           "OEM specific",
           "OEM specific",
           "OEM specific",
           "Attn",
           "CrSel",
           "ExSel",
           "Erase EOF",
           "Play",
           "Zoom",
           "reserved",
           "PA1 key" ,
           "clear"
};

// main threads init
void toad::launch_threads() {
#ifndef _DEBUG
    std::thread(&c_mouseHook::thread, p_mouseHook.get()).detach();
#endif // !_DEBUG

//    std::thread(&c_clicker::thread, p_clicker.get()).detach();
//    std::thread(&c_right_clicker::thread, p_right_clicker.get()).detach();

    std::thread(toad::misc::window_scanner).detach();
}

bool cursed_binds = false;
/// <summary>
/// Reads options.txt and converts the LWJGL keycodes to Windows keycodes.
/// </summary>
/// <param name="hotkeys">LWJGL hotkeys</param>
/// <returns>The windows keycodes in int types</returns>
std::vector<int> toad::mapHotkeys(std::vector<std::string>& hotkeys)
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
            log_error(e.what());
        }
        catch(const std::out_of_range& e)
        {
            log_error(e.what());
        }

        auto it = mappedvalues.find(keycode == -1 ? j : keycode);
        if (it != mappedvalues.end())
            vec.push_back(it->second);
    }

    return vec;
}

using convert_type = std::codecvt_utf8<wchar_t>;
std::wstring_convert<convert_type, wchar_t> converter;

bool toad::init_toad()
{
    log_debug("initializing");

    WCHAR buf[MAX_PATH];
    GetModuleFileName(NULL, buf, MAX_PATH);
    std::wstring p(buf);
    toad::misc::exePath = converter.to_bytes(p);
    int j = 0;
    for (int i = toad::misc::exePath.length(); i > 0; i--)
    {
        j++;
        if (toad::misc::exePath[i] == '\\') {
            toad::misc::exePath.erase(toad::misc::exePath.length() - j + 1);
            break;
        }
    }

    log_debug(toad::misc::exePath);

    if (!p_SoundPlayer->get_all_outputDevices(toad::clicksounds::audiodevList))
    {
        log_error("failed to retrieve audio output devices");
        return false;
    }

    log_debug("getting options.txt");

    const char* optionsPath = getenv("APPDATA");

    std::string pathstr = std::string(optionsPath);
    pathstr.append("\\.minecraft\\options.txt");

    std::vector<std::string> mc_options;

    std::ifstream f;
    f.open(pathstr, std::ios::in);
    if (f.is_open()) {
        log_debug("found minecraft options.txt");
        toad::optionsFound = true;
        std::string* buf = new std::string;
        while (std::getline(f, *buf)) {
            mc_options.push_back(*buf);
        }
        delete buf;
        f.close();
    }
    else
    {
        log_error("failed to retrieve minecraft options.txt");
        toad::optionsFound = false;
        goto LABLE_THREADLAUNCH;
    }

    log_debug("retrieving hotbar settings");
    //remove options except for hotbar settings
    
    int index = 0;
    for (unsigned int i = 0; i < mc_options.size(); i++)
    {
        log_debug(mc_options[i]);
        if (mc_options[i].find("key_key.hotbar") != std::string::npos)
        {
            index = i;
            log_ok(index);
            break;
        }
    }
    if (index == 0)
    {
        log_error("couldn't find hotbar settings");
        toad::optionsFound = false;
        goto LABLE_THREADLAUNCH;
    }
    
    mc_options.erase(mc_options.begin() + index + 9, mc_options.end());
    mc_options.erase(mc_options.begin(), mc_options.begin() + index);
    /*mc_options.erase(mc_options.end() - 16, mc_options.end());
    mc_options.erase(mc_options.begin(), mc_options.begin() + 78);*/

    for (auto& mc_option : mc_options)
	    for (int i = 0; i < mc_option.size(); i++)
            if (mc_option[i] == ':')
            {
                for (int j = 0; j < i + 1;j++) log_debug(mc_options[i].at(j));
                mc_option.erase(0, i + 1);
            }

    log_debug("mapping hotkeys");

    // map LWJGL keys to virtual keycodes
    toad::hotbarVKCodes = toad::mapHotkeys(mc_options);

    log_debug("mapped hotkeys");

    p_SoundPlayer->get_all_compatible_sounds(toad::clicksounds::soundslist, toad::clicksounds::selectedClicksounds);


LABLE_THREADLAUNCH:
    log_debug("initializing threads");
    toad::launch_threads();
    log_debug("threads initialized");

    toad::is_running = true;

    return true;
}

bool toad::window_is_focused(const HWND& window)
{
    return GetForegroundWindow() == window;
}

std::vector<std::string> toad::getAllFilesExt(const std::filesystem::path& path, const char* ext, const bool includeExt)
{
    std::vector <std::string> vec = {};
    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        if (entry.path().extension() == std::move(ext))
            includeExt ? vec.push_back(entry.path().stem().string() + ext) : vec.push_back(entry.path().stem().string());
    }
    return vec;
}
