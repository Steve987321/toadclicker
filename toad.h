#pragma once
#include <string>
#include <iostream>
#include <windows.h>
#include <chrono>
#include <thread>
#include <random>
#include <vector>
#include <TlHelp32.h>

#include "logger.h"


#define WINDOW_WIDTH 700
#define WINDOW_HEIGHT 400

//global vars&functions
namespace toad
{
    struct ProcInfo 
    {
        DWORD pid; std::string pname; HWND hwnd;
        ProcInfo(DWORD PID, std::string PNAME) : pid(PID), pname(PNAME) { hwnd = NULL; }
        ProcInfo(DWORD PID, std::string PNAME, HWND HWND) : pid(PID), pname(PNAME), hwnd(HWND) {} 
    };

    namespace clicker {
        namespace r {
            inline bool right_enabled = false;
            inline bool right_inventory = false;
            inline bool right_only_inventory = false;
            inline int right_mincps = 10;
            inline int right_maxcps = 15;
            inline std::string right_key = "none";
            inline int right_keycode = 0;
        }
        inline bool higher_cps = false;
        inline bool enabled = false;
        inline bool cursor_visible = false;
        inline bool blatant_mode = false;
        inline bool click_sounds = false;
        inline bool inventory = false;
        inline bool slot_whitelist = false;
        inline bool rmb_lock = false;
        inline int mincps = 10;
        inline int maxcps = 15;
        inline int keycode = 0;
        inline std::string key = "none";
        inline bool whitelisted_slots[8];
        inline int curr_slot = 0;
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

        static const char* items[]{ "Active Window","Minecraft", "Custom Window" };
        inline int selectedClickWindow = 1; //TODO: make this enum?
        inline char custom_windowTitle[100] = "";
        inline std::vector<toad::ProcInfo> procList = {};
        inline DWORD pid = 0;

        BOOL get_window_list();
        void window_scanner();
        void show(HWND window);
        void hide(HWND window);
    }

    inline HWND clicking_window = NULL;

    bool window_is_focused(const HWND& window);
    
    void renderUI(const HWND& hwnd, bool& done);
    void launch_threads();
    void hotkey_handler(HWND hwnd);

    inline bool is_running = false;

    bool init();
    inline bool optionsFound = false;
    
    std::vector<int> mapHotkeys(std::vector<std::string>& hotkeys);
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
