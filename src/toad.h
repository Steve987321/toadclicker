#pragma once

#include <json.hpp>
#include "logger.h"
#include "SoundPlayer.h"
#include "clicker/Clicker.h"
#include "clicker/MouseHook.h"
#include "clicker/ClickRecorder.h"
#include "clicker/DoubleClicker.h"

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
            inline int right_keycode = 0;
            inline std::string right_key = "none";
            inline int right_selectedEnableOption = 0;
            constexpr const char* right_enableOptions_c[] = { "Toggle to Enable","Hold to Click","Toggle to Click" };
        }

        inline bool one_slider              = false;
        inline bool higher_cps              = false;
        inline bool enabled                 = false;
        inline bool cursor_visible          = false;
        inline bool blatant_mode            = false;
        inline bool inventory               = false;
        inline bool slot_whitelist          = false;
        inline bool rmb_lock                = false;
        inline bool use_recorded_delays     = false;

        inline int cps                      = 10;
        inline int mincps                   = 10;
        inline int maxcps                   = 15;
        inline int keycode                  = 0;

        inline std::string key = "none";
        inline bool whitelisted_slots[8];
        inline int curr_slot = 0;
        inline int selectedEnableOption = 0;
        constexpr const char* enable_options_c[] = { "Toggle to Enable","Hold to Click","Toggle to Click" };
    }

    namespace double_clicker
    {
        inline bool enabled = false;
        inline int delay = 50;
        inline int chance = 75;
        inline int min_interval = 4;
        inline int max_interval = 7;

        inline std::string key = "none";
        inline int keycode = 0;
    }

    namespace jitter {
        void do_jitter();
        inline bool enable = false;
        inline int intensity_X = 3;
        inline int intensity_Y = 3;
        inline int chance = 80;
    }

    namespace clicksounds
    {
        inline bool play = false;
        inline bool enabled = false;
        inline bool use_customOutput = false;
        inline int selectedDeviceID = 0;

        inline std::string selectedDevice = "none";
        inline std::vector<std::string> audiodevList = {};

        inline std::vector<std::string> soundslist = {};
        inline std::vector<std::string> selectedClicksounds = {};

        inline int volumePercent = 50;
        inline bool randomizeVol = false;

        inline int volmin = 25, volmax = 50;
    }

    namespace misc {

        inline bool use_mouseEvent = false;

        inline bool beep_on_toggle = false;
        inline bool window_hidden = false;
        inline bool compatibility_mode = false;
        inline std::string hide_key = "none";
        inline int keycode = 0;

        constexpr const char* window_options_c[]{ "Active Window","Minecraft","Custom Window" };
        inline int selectedClickWindow = 1; 
        inline char custom_windowTitle[100] = "";
        inline std::vector<toad::ProcInfo> procList = {};
        inline DWORD pid = 0;

        inline constexpr const char* server_presets_c[]{ "Hypixel", "MMC","Lunar" };
        inline int selectedPreset = 0;
        void loadConfig(const std::string path);

        void saveConfig(std::string name); // create and load configs

        inline std::vector<std::string> ConfigList = {};
        inline int selectedConfig = 0;

        std::vector <std::string> GetAllToadConfigs(std::filesystem::path path);

        inline std::string exePath;

        BOOL get_window_list();
        void window_scanner();
        void show(HWND window);
        void hide(HWND window);
    }

    namespace clickrecorder
    {
        enum class recordStatus
        {
            AWAITING_FOR_CLICK,
            RECORDING,
            NOT_RECORDING,
            SKIPPING_NEXT_CLICK
        };

        inline bool enabled = false;
        inline bool custom_extension = false;
        inline bool auto_unbind = false;
        inline bool skip_on_delay = false;
        inline bool inventory = false;
        inline bool playback_enabled = false;
        inline bool randomize_start_point = false;

        inline recordStatus record_status = recordStatus::NOT_RECORDING;
       
        inline double skip_delay = 1.0; // in seconds

        inline int total_clicks = 0;
        inline double average_cps = 0.0;
        inline float fulltime = 0.f;

        inline float multiplier = 1.0f;

        inline int keycode = 0;
        inline std::string key = "none";
        
        inline int keycode_playback = 0;
        inline std::string key_playback = "none";

        inline int click_start_point = 0;
        extern bool is_start_point_valid();

        inline std::vector<float> click_delays = {};
    }

    namespace theme
    {
        inline float main_col[3] = { 0.0f / 255, 82.0f / 255, 22.0f / 255 };
        inline float main_col_dark[3];
        inline float main_col_darker[3];
        inline float main_col_light[3];

        inline bool hue_loop_mode = false;
        inline float speed = 0.05f;
        extern void do_hue_loop(float deltatime);
    }

    inline HWND clicking_window = NULL;

    std::vector<int> mapHotkeys(std::vector<std::string>& hotkeys);

    bool init_toad();
    bool window_is_focused(const HWND& window);
    void renderUI(const HWND& hwnd);
    void launch_threads();
    void hotkey_handler(const HWND& hwnd);

    inline std::atomic_bool is_running = false;
    inline bool optionsFound = false;

    inline bool clickplayback_thread_exists = false;
    inline bool clickrecord_thread_exists = false;

    constexpr const char* APP_VER = "1.7.7";

    inline std::vector<int> hotbarVKCodes;

    extern std::string keys[];

    extern std::vector<std::string> getAllFilesExt(const std::filesystem::path& path, const char* ext, const bool includeExt = false);

    inline int random_float(float min, float max) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(min, max);
        return dis(gen);
    }

    inline int random_int(int min, int max) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> distribution(min, max);
        return distribution(gen);
    }

    // https://blat-blatnik.github.io/computerBear/making-accurate-sleep-function/
    inline void preciseSleep(double seconds) {
        static double estimate = 5e-3;
        static double mean = 5e-3;
        static double m2 = 0;
        static int64_t count = 1;

        while (seconds > estimate) {
            auto start = std::chrono::high_resolution_clock::now();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            auto end = std::chrono::high_resolution_clock::now();

            double observed = (end - start).count() / 1e9;
            seconds -= observed;

            ++count;
            double delta = observed - mean;
            mean += delta / count;
            m2 += delta * (observed - mean);
            double stddev = sqrt(m2 / (count - 1));
            estimate = mean + stddev;
        }

        // spin lock
        auto start = std::chrono::high_resolution_clock::now();
        while ((std::chrono::high_resolution_clock::now() - start).count() / 1e9 < seconds);
    }
}
