#include "toad.h"
#include "Clicker.h"
#include "slotWhitelist.h"

#include <fstream>
#include <map>

std::string toad::keys[] = {
       "Left mouse",
       "Right mouse",
       "Control - break",
       "Middle mouse button",
       "X1 mouse",
       "X2 mouse",
       "",
       "BACKSPACE",
       "TAB",
       "",
       "",
       "CLEAR",
       "ENTER",
       "",
       "",
       "SHIFT",
       "CTRL",
       "ALT",
       "PAUSE",
       "CAPS LOCK",
       "IME Kana",
       "",
       "IME Junja",
       "IME final",
       "IME Hanja kanji",
       "",
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
       " ",
       " ",
       " ",
       " ",
       " ",
       " ",
       " ",
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
       "Left Windows key(Microsoft� Natural� keyboard",
       "Right Windows key(Natural keyboard",
       "Applications key(Natural keyboard",
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
       "F12"
};

void toad::launch_threads() {
    //thread init
    std::thread(&c_slotWhitelist::thread, p_slotWhitelist.get()).detach();
    std::thread(&c_clicker::thread, p_clicker.get()).detach();
    std::thread(&c_right_clicker::thread, p_right_clicker.get()).detach();
}

/// <summary>
/// Reads options.txt and converts the LWJGL keycodes to Windows keycodes.
/// </summary>
/// <param name="hotkeys">LWJGL hotkeys</param>
/// <returns>The windows keycodes in int types</returns>
std::vector<int> toad::mapHotkeys(std::vector<std::string>& hotkeys)
{
    //options.txt minecraft getenv
    //const char* optionsPath = getenv("APPDATA");
    //https://minecraft.fandom.com/el/wiki/Key_codes

    //first: LWGL, SECONDS: WIndows
    const std::map<int, int> mappedvalues{
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
     // { 15, VK_TAB }, // tab
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

    for (int i = 0; i < hotkeys.size(); i++)
    {
        auto j = mappedvalues.find(std::stoi(hotkeys[i]));
        if (j != mappedvalues.end())
            vec.push_back(j->second);
    }

    return vec;
}

/// <summary>
/// Initialize the autoclicker
/// </summary>
/// <returns>Wether we have sucessfully initalized</returns>
bool toad::init()
{
    //logger
    log_debug("initializing");

    //get options.txt
    log_debug("getting options.txt");

    const char* optionsPath = getenv("APPDATA");

    std::string pathstr = std::string(optionsPath);
    pathstr.append("\\.minecraft\\options.txt");

    std::vector<std::string> mc_options;

    std::ifstream f;
    f.open(pathstr, std::ios::in);
    if (f.is_open()) {
        log_debug("found options.txt");

        std::string buf;
        while (std::getline(f, buf)) {
            mc_options.push_back(buf);
        }
        f.close();
    }
    else
    {
        log_error("failed to retrieve options.txt");
        return false;
    }

    log_debug("retrieving hotbar settings");
    //remove options except for hotbar settings
    mc_options.erase(mc_options.end() - 16, mc_options.end());
    mc_options.erase(mc_options.begin(), mc_options.begin() + 78);

    for (int i = 0; i < mc_options.size(); i++)
        for (int j = 0; j < mc_options[i].size(); j++)
            if (mc_options[i][j] == ':')
                mc_options[i].erase(0, j + 1);

    log_debug("mapping hotkeys");
    // map LWJGL keys to virtual keycodes
    toad::hotbarVKCodes = toad::mapHotkeys(mc_options);

    log_debug("mapped hotkeys");

    log_debug("initializing threads");
    toad::launch_threads();
    log_debug("threads initialized");

    return true;
}

bool toad::window_is_focused(HWND& window)
{
    return GetForegroundWindow() == window;
}