#pragma once

#include "Helpers.h"
#include "Keys.h"
#include "Logger.h"
#include "SoundPlayer.h"
#include "Config.h"
#include "clicker/Misc.h"
#include "clicker/Clicker.h"
#include "clicker/MouseHook.h"
#include "clicker/ClickRecorder.h"
#include "clicker/DoubleClicker.h"
#include "clicker/Jitter.h"

//global vars&functions
namespace toad
{
    namespace clicker {
        namespace r {
            inline bool right_enabled = false;
            inline bool right_inventory = false;
            inline bool right_only_inventory = false;
            inline int right_mincps = 10;
            inline int right_maxcps = 15;
            inline int right_keycode = 0;
            inline std::string right_key = "none";
            inline int right_selected_enable_option = 0;
            constexpr const char* right_enable_options[] = { "Toggle to Enable", "Hold to Click", "Toggle to Click" };
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
        inline bool whitelisted_slots[9];
        inline int curr_slot = 0;
        inline int selected_enable_option = 0;
        constexpr const char* enable_options[] = { "Toggle to Enable", "Hold to Click", "Toggle to Click" };
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

    namespace jitter 
    {
        inline bool enabled = false;
        inline int intensity_X = 3;
        inline int intensity_Y = 3;
        inline int chance = 80;
    }

    namespace clicksounds
    {
        inline bool enabled = false;
        inline bool use_custom_output = false;
        inline int selected_device_ID = 0;

        inline std::string selected_device = "none";
        inline std::vector<std::string> audio_device_list = {};

        inline std::vector<std::string> sounds_list = {};

		// stores the selected sounds by full sound file path. Pair order: down, up
		inline std::vector<std::pair<std::filesystem::path, std::filesystem::path>> selected_clicksounds = {};

        inline int volume_percent = 50;
        inline bool randomize_volume = false;

        inline int volume_min = 25;
        inline int volume_max = 50;
    }

    namespace misc 
    {
        inline bool use_mouse_event = false;

        inline bool beep_on_toggle = false;
        inline bool window_hidden = false;
        inline bool compatibility_mode = false;
        inline std::string hide_key = "none";
        inline int keycode = 0;

        constexpr const char* window_options[]{ "Active Window", "Minecraft", "Custom Window" };
        inline int selected_click_window = 1; 
        inline char custom_window_title[100] = "";
        inline DWORD pid = 0;

        inline constexpr const char* server_presets[]{ "Hypixel", "MMC","Lunar" };
        inline int selected_preset = 0;
        inline std::vector<std::string> config_list = {};
        inline int selected_config = 0;

        inline std::filesystem::path exe_path;
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

		bool is_start_point_valid();

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

        inline std::vector<float> click_delays = {};
    }

    namespace theme
    {
		void do_hue_loop(float deltatime);

        inline float main_col[3] = { 0.0f / 255, 82.0f / 255, 22.0f / 255 };
        inline float main_col_dark[3];
        inline float main_col_darker[3];
        inline float main_col_light[3];

        inline bool hue_loop_mode = false;
        inline float speed = 0.05f;
    }

    // Sets up the clicker
    bool init_toad();

    // Starts important threads
    void launch_threads();

	// handles all hotkey presses and binding something to a key
    void hotkey_handler(const HWND& hwnd);

	inline HWND clicking_window = nullptr;

    inline std::atomic_bool is_running = false;
    inline bool options_found = false;

    constexpr const char* APP_VER = "1.8.0 WIP";

    inline std::vector<int> hotbar_virtual_keycodes;
}
