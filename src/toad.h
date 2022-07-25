#pragma once

#include "logger.h"
#include <nlohmann/json.hpp>

//global vars&functions
namespace toad
{
    struct ProcInfo 
    {
        DWORD pid; std::string pname; HWND hwnd;
        ProcInfo(DWORD PID, std::string PNAME) : pid(PID), pname(PNAME) { hwnd = NULL; }
        ProcInfo(DWORD PID, std::string PNAME, HWND HWND) : pid(PID), pname(PNAME), hwnd(HWND) {} 
    };

    struct Preset {
        bool rEnabled;

        int lcpsmin, lcpsmax;
        int rcpsmin, rcpsmax;

        bool llmbLock, linventory, lblatant, lhigherCPS, rInventory, rOnlyInventory;
    };

    namespace clicker {
        namespace r {
            inline bool right_enabled = false;
            inline bool right_inventory = false;
            inline bool right_only_inventory = false;
            inline int right_mincps = 10;
            inline int right_maxcps = 15;
            inline int right_keycode = 0;
            inline std::string right_key = "none";
            inline int right_selectedEnableOption = 0;
            static const char* right_enableOptions_c[] = { "Toggle to Enable","Hold to Click","Toggle to Click" };
        }
        inline bool higher_cps = false;
        inline bool enabled = false;
        inline bool cursor_visible = false;
        inline bool blatant_mode = false;
//      inline bool click_sounds = false;
        inline bool inventory = true;
        inline bool slot_whitelist = false;
        inline bool rmb_lock = false;
        inline int mincps = 10;
        inline int maxcps = 15;
        inline int keycode = 0;
        inline std::string key = "none";
        inline bool whitelisted_slots[8];
        inline int curr_slot = 0;
        inline int selectedEnableOption = 0;
        static const char* enable_options_c[] = { "Toggle to Enable","Hold to Click","Toggle to Click" };
    }
    namespace jitter {
        void do_jitter();
        inline bool enable = false;
        inline int intensity_X = 3;
        inline int intensity_Y = 3;
        inline int chance = 80;
    }
    namespace misc {
        inline bool beep_on_toggle = false;
        inline bool window_hidden = false;
        inline std::string hide_key = "none";
        inline int keycode = 0;

        static const char* window_options_c[]{ "Active Window","Minecraft","Custom Window" };
        inline int selectedClickWindow = 1; //TODO: make this enum?
        inline char custom_windowTitle[100] = "";
        inline std::vector<toad::ProcInfo> procList = {};
        inline DWORD pid = 0;

        //config system & presets
        inline static toad::Preset presets[] = 
        {
            //Hypixel
            {
                false,		/*right_enabled*/

                15, 18,		/*min, max lcps*/
                15, 18,		/*min, max rcps*/
                
                true,		/*lmb_lock*/
                false,		/*inventory*/
                false,		/*blatant mode*/
                true,		/*higher cps*/

                false,		/*right inventory*/
                false		/*right only inventory*/
            },
            //minemen
            {
                true,   	/*right_enabled*/

                10, 13,		/*min, max lcps*/
                10, 12,		/*min, max rcps*/

                false,		/*lmb_lock*/
                false,		/*inventory*/
                false,		/*blatant mode*/
                false,		/*higher cps*/

                true,		/*right inventory*/
                true		/*right only inventory*/
            },
            //lunar
            {
                true,		/*right_enabled*/

                10, 15,		/*min, max lcps*/
                10, 12,		/*min, max rcps*/

                false,		/*lmb_lock*/
                false,		/*inventory*/
                false,		/*blatant mode*/
                false,		/*higher cps*/

                true,		/*right inventory*/
                true		/*right only inventory*/
            }
        };

        inline static const char* server_presets_c[]{ "Hypixel", "MMC","Lunar" };
        inline int selectedPreset = 0;
        void loadConfig(const toad::Preset& preset);
        void loadConfig(const std::string path);

        void createConfig(std::string name);
        void saveConfig(std::string name);
        
        inline std::vector<std::string> ConfigList = {};
        inline int selectedConfig = 0;

        std::vector <std::string> GetAllToadConfigs(std::filesystem::path path);

        inline std::string exePath;

        BOOL get_window_list();
        void window_scanner();
        void show(HWND window);
        void hide(HWND window);
    }

    inline HWND clicking_window = NULL;

    bool init_toad();
    bool window_is_focused(const HWND& window);
    void renderUI(const HWND& hwnd);
    void launch_threads();
    void hotkey_handler(HWND hwnd);

    inline bool is_running = false;
    inline bool optionsFound = false;

    static const char* APP_VER = "1.2.1";
    
    static std::vector<int> mapHotkeys(std::vector<std::string>& hotkeys);
    inline std::vector<int> hotbarVKCodes;

    extern std::string keys[];

    inline int random_float(float min, float max) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(min, max);
        return dis(gen);
    }
    inline int random_int(int min, int max) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> distribution(min, max);
        return distribution(gen);
    }
}
