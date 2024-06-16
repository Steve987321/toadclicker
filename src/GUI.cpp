#include "pch.h"
#include "Application.h"
#include "Toad.h"
#include "GUI.h"
#include "imgui.h"
#include <imgui_internal.h>

#include "Clicker/Jitter.h"

bool showProcessList    = false;
bool binding            = false;

std::once_flag flag;
std::once_flag onceFlag;
float h = 0, s = 0, v = 0;

float rgbModeVal = 0;
float previousColor[4] = {0,0,0,0};

namespace toad
{

void theme::do_hue_loop(float deltatime)
{
    rgbModeVal += speed * deltatime;
    if (rgbModeVal >= 1)
        rgbModeVal = 0;
    ImGui::ColorConvertHSVtoRGB(rgbModeVal, s, v, theme::main_col[0], theme::main_col[1], theme::main_col[2]);
    
}

void hotkey_handler(const HWND& window) {
	int k = 0;

    if (!binding) {
        //misc Hide and Unhide
        if ((GetAsyncKeyState(misc::keycode) & 1) && (k == 0))
        {
            if (misc::window_hidden) { misc::show(window); misc::window_hidden = false; }
            else if (!misc::window_hidden) { misc::hide(window); misc::window_hidden = true; }
            k = 1;
        }
        else if (GetAsyncKeyState(misc::keycode) == 0) k = 0;

        //Clicker L&R
        switch (clicker::selected_enable_option)
        {
        case 2:
        case 0:
            if (GetAsyncKeyState(clicker::keycode) & 1) {
                if (clicker::keycode == clicker::r::right_keycode)
                {
                    if (clicker::r::right_enabled) p_rightClicker->StopThread();
                    else p_rightClicker->StartThread();
                    clicker::r::right_enabled = !clicker::r::right_enabled;
                }
                if (misc::beep_on_toggle) Beep(350, 100);
                if (clicker::enabled) p_clicker->StopThread();
                else p_clicker->StartThread();
                clicker::enabled = !clicker::enabled;
            }
            break;
        default:
            break;
        }
        switch (clicker::r::right_selected_enable_option)
        {
        case 2:
        case 0:
            if (GetAsyncKeyState(clicker::r::right_keycode) & 1) {
                if (misc::beep_on_toggle) Beep(350, 100);
                if (clicker::r::right_enabled) p_rightClicker->StopThread();
                else p_rightClicker->StartThread();
                clicker::r::right_enabled = !clicker::r::right_enabled;
            }
            break;
        default:
            break;
        }  

        //click recorder 
        if (GetAsyncKeyState(clickrecorder::keycode) & 1) {
            if (clickrecorder::enabled)
            {
                if (clickrecorder::auto_unbind)
                {
                    clickrecorder::keycode = 0;
                    clickrecorder::key = "none";
                }
                clickrecorder::click_start_point = 0;
                clickrecorder::record_status = clickrecorder::recordStatus::NOT_RECORDING;
            }
            else
            {
                clickrecorder::total_clicks = 0;
                clickrecorder::record_status = clickrecorder::recordStatus::AWAITING_FOR_CLICK;
                p_clickRecorder->Reset();
            }

            clickrecorder::enabled = !clickrecorder::enabled;

			if (clickrecorder::enabled)
				p_clickRecorder->StartRecordingThread();
			else
				p_clickRecorder->StopRecordingThread();
        }

        //click playback
        if (GetAsyncKeyState(clickrecorder::keycode_playback) & 1 && !clickrecorder::click_delays.empty())
        {
            clickrecorder::playback_enabled = !clickrecorder::playback_enabled;

            if (clickrecorder::playback_enabled)
                p_clickRecorder->StartPlaybackThread();
            else
                p_clickRecorder->StopPlaybackThread();
        }

        // double clicker
        if (GetAsyncKeyState(double_clicker::keycode) & 1)
        {
            double_clicker::enabled = !double_clicker::enabled;

            if (double_clicker::enabled)
            {
                LOG_DEBUG("starting thread");
                p_doubleClicker->StartThread();
            }
            else
            {
                LOG_DEBUG("stopping thread");
                p_doubleClicker->StopThread();
            }
        }
    }

    // when binding to a button
    else if (binding) {
        // (1)lmb - (123)f12
        for (int i = 3; i < 251; i++) {
            if (clicker::key == "..") {
                if (GetAsyncKeyState(i) & 0x8000) {
                    if (i == VK_ESCAPE) { clicker::key = "none"; binding = false; clicker::keycode = 0; }
                    else { clicker::key = keys[i - 1]; clicker::keycode = i; }
                }
                if (clicker::key != "..") binding = false;
            }
            else if (clicker::r::right_key == "..") {
                if (GetAsyncKeyState(i) & 0x8000) {
                    if (i == VK_ESCAPE) { clicker::r::right_key = "none"; binding = false; clicker::r::right_keycode = 0; }
                    else { clicker::r::right_key = keys[i - 1]; clicker::r::right_keycode = i; }
                }
                if (clicker::r::right_key != "..") binding = false;
            }
            else if (misc::hide_key == "..") {
                if (GetAsyncKeyState(i) & 0x8000) {
                    if (i == VK_ESCAPE) { misc::hide_key = "none"; binding = false; misc::keycode = 0; }
                    else { misc::hide_key = keys[i - 1]; misc::keycode = i; }
                }
                if (misc::hide_key != "..") binding = false;
            }
            else if (clickrecorder::key == "..") {
                if (GetAsyncKeyState(i) & 0x8000) {
                    if (i == VK_ESCAPE) { clickrecorder::key = "none"; binding = false; clickrecorder::keycode = 0; }
                    else { clickrecorder::key = keys[i - 1]; clickrecorder::keycode = i; }
                }
                if (clickrecorder::key != "..") binding = false;
            }
            else if (clickrecorder::key_playback == "..") {
                if (GetAsyncKeyState(i) & 0x8000) {
                    if (i == VK_ESCAPE) { clickrecorder::key_playback = "none"; binding = false; clickrecorder::keycode_playback = 0; }
                    else { clickrecorder::key_playback = keys[i - 1]; clickrecorder::keycode_playback = i; }
                }
                if (clickrecorder::key_playback != "..") binding = false;
            }
            else if (double_clicker::key == "..") {
                if (GetAsyncKeyState(i) & 0x8000) {
                    if (i == VK_ESCAPE) { double_clicker::key = "none"; binding = false; double_clicker::keycode = 0; }
                    else { double_clicker::key = keys[i - 1]; double_clicker::keycode = i; }
                }
                if (double_clicker::key != "..") binding = false;
            }
        }
    }
}

void render_ui(const HWND& hwnd) 
{
	static ImGuiIO* io = &ImGui::GetIO();

    hotkey_handler(hwnd);

    ImGui::SetNextWindowSize(ImVec2(500, WINDOW_HEIGHT));
    ImGui::Begin("Toad", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoNavInputs);

    ui_decorations();
    if (theme::hue_loop_mode)
        theme::do_hue_loop(io->DeltaTime);

    ImGui::TextColored(ImVec4(theme::main_col[0], theme::main_col[1], theme::main_col[2], 1.f), "toad");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1), "minecraft");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1), APP_VER);
#ifdef _DEBUG
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1), "Debug Mode");
#endif

	int current_tab = 0;
    if (ImGui::BeginTabBar("##tabbar"))
    {
        if (ImGui::BeginTabItem("  clicker  ", false))
        {
            current_tab = 0;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("  configs  ", false))
        {
            if (misc::config_list.empty())
                misc::config_list = config::get_all_toad_configs(misc::exe_path);

            else
            {
                misc::config_list.clear();
                misc::config_list = config::get_all_toad_configs(misc::exe_path);
            }

            current_tab = 1;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("  recorder  ", false))
        {
            current_tab = 2;
            ImGui::EndTabItem();
        } 
        if (ImGui::BeginTabItem("   misc    ", false))
        {
            current_tab = 3;
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::SetCursorPosX(20);
    ImGui::SetCursorPosY(60);

    //clicker
    if (current_tab == 0) {

        static int clickertab = 0;
        static int clickertab2 = 0;
        ImGui::BeginChild("clicker", ImVec2(ImGui::GetWindowSize().x / 2 - 30, 300), true);
        ImGui::SetCursorPosX(ImGui::GetWindowSize().x / 2 - 50);
        ImGui::BeginTabBar("##ClickerTabs");

        if (clickertab2 != 0) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.45, 0.45, 0.45, 1));
        if (ImGui::BeginTabItem("left")) { clickertab = 0; ImGui::EndTabItem(); }
        if (clickertab2 != 0) ImGui::PopStyleColor();

        if (clickertab2 != 1) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.45, 0.45, 0.45, 1));
        if (ImGui::BeginTabItem("right")) { clickertab = 1; ImGui::EndTabItem(); }
        if (clickertab2 != 1)ImGui::PopStyleColor();
        clickertab2 = clickertab;
        ImGui::EndTabBar();

        //LEFT CLICKER
        if (clickertab == 0)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 3.5f));

            ImGui::BeginChild("left", ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y - 50));

//          ImGui::SetCursorPosX((ImGui::GetWindowSize().x / 2) - ImGui::CalcTextSize("left").x + 10);
//          ImGui::TextColored(ImVec4(0.47f, 0.47f, 0.47f, 1.f), "left");

            ImGui::Separator();

            if (ImGui::Checkbox("##Enable Left", &clicker::enabled))
            {
                if (clicker::enabled)
                    p_clicker->StartThread();
                else p_clicker->StopThread();
            }

            ImGui::SameLine();
            ImGui::Text("enable");
            ImGui::SameLine();
            ImGui::TextColored(ImColor(51, 51, 51), "[%s]", &clicker::key);

            if (ImGui::IsItemClicked()) { clicker::key = ".."; binding = true; }
            
            ImGui::Combo("##EnableOptions", &clicker::selected_enable_option, clicker::enable_options, IM_ARRAYSIZE(clicker::enable_options));
            ImGui::Spacing();

            if (misc::use_mouse_event && clicker::selected_enable_option == 0) clicker::selected_enable_option = 1;
            
            if (clicker::one_slider)
            {
                ImGui::Text("average cps");
                ImGui::SliderInt("##cps", &clicker::cps, 5, 20, "%dcps");
            }
            else
            {
                int cps_limit = clicker::blatant_mode ? 100 : 20;

                ImGui::Text("min");
               
                if (ImGui::SliderInt("##Min", &clicker::mincps, 5, cps_limit, "%dcps"))
                {
                    if (clicker::mincps > clicker::maxcps)
                    {
                        clicker::maxcps = clicker::mincps;
                    }
                }

                ImGui::Text("max");
                if (ImGui::SliderInt("##Max", &clicker::maxcps, 5, cps_limit, "%dcps"))
                {
                    if (clicker::maxcps < clicker::mincps)
                    {
                        clicker::mincps = clicker::maxcps;
                    }
                }
            }

            ImGui::Spacing();

            ImGui::Checkbox("rmb lock", &clicker::rmb_lock);
            ImGui::Checkbox("when mouse visible", &clicker::inventory);
            ImGui::Checkbox("blatant mode", &clicker::blatant_mode);
            if (ImGui::IsItemClicked())
            {
                if (clicker::maxcps > 20) clicker::maxcps = 20;
                if (clicker::mincps > 20) clicker::mincps = 20;
            }

            ImGui::Checkbox("prioritize higher cps", &clicker::higher_cps);
            ImGui::Checkbox("one slider", &clicker::one_slider);

            if (!options_found)
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha / 2);
            }
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1.f, 0.5f));
            ImGui::Checkbox("slot whitelist", &clicker::slot_whitelist);
            if (clicker::slot_whitelist) {
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.7f, 1.f));
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.2f, 3.f));
                ImGui::SetCursorPosX(10);
                ImGui::Checkbox("##slot1", &clicker::whitelisted_slots[0]); ImGui::SameLine();
                ImGui::Checkbox("##slot2", &clicker::whitelisted_slots[1]); ImGui::SameLine();
                ImGui::Checkbox("##slot3", &clicker::whitelisted_slots[2]); ImGui::SameLine();
                ImGui::Checkbox("##slot4", &clicker::whitelisted_slots[3]); ImGui::SameLine();
                ImGui::Checkbox("##slot5", &clicker::whitelisted_slots[4]); ImGui::SameLine();
                ImGui::Checkbox("##slot6", &clicker::whitelisted_slots[5]); ImGui::SameLine();
                ImGui::Checkbox("##slot7", &clicker::whitelisted_slots[6]); ImGui::SameLine();
                ImGui::Checkbox("##slot8", &clicker::whitelisted_slots[7]); ImGui::SameLine();
                ImGui::Checkbox("##slot9", &clicker::whitelisted_slots[8]);
                ImGui::PopStyleVar();
                ImGui::PopStyleVar();
            }

            ImGui::PopStyleVar();

            if (!options_found)
            {
                ImGui::PopItemFlag();
                ImGui::PopStyleVar();
            }

            ImGui::PopStyleVar();

            ImGui::EndChild();
        }
        //RIGHT CLICKER
        else if (clickertab == 1)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 3.5f));

            ImGui::BeginChild("right", ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y - 50));

            ImGui::Separator();

            if (ImGui::Checkbox("##Enable right", &clicker::r::right_enabled))
            {
                if (clicker::r::right_enabled)
                    p_rightClicker->StartThread();
                else p_rightClicker->StopThread();
            }

            ImGui::SameLine(); 
            ImGui::Text("enable"); 
            ImGui::SameLine();

            ImGui::PushID("rightBindText");
            ImGui::TextColored(ImColor(51, 51, 51), "[%s]", &clicker::r::right_key);
            ImGui::PopID();

            if (ImGui::IsItemClicked()) { clicker::r::right_key = ".."; binding = true; }

            ImGui::Combo("##EnableOptionsRight", &clicker::r::right_selected_enable_option, clicker::r::right_enable_options, IM_ARRAYSIZE(clicker::r::right_enable_options));

            if (misc::use_mouse_event && clicker::r::right_selected_enable_option == 0) clicker::r::right_selected_enable_option = 1;

            ImGui::Spacing();

            ImGui::Text("min");
            if (ImGui::SliderInt("##Min right", &clicker::r::right_mincps, 5, 90, "%dcps"))
            {
                if (clicker::r::right_mincps > clicker::r::right_maxcps)
                {
                    clicker::r::right_maxcps = clicker::r::right_mincps;
                }
            }
            ImGui::Text("max");
            if (ImGui::SliderInt("##Max right", &clicker::r::right_maxcps, 5, 90, "%dcps"))
            {
                if (clicker::r::right_maxcps < clicker::r::right_mincps)
                {
                    clicker::r::right_mincps = clicker::r::right_maxcps;
                }
            }

            ImGui::Spacing();

            ImGui::Checkbox("##Inventory right", &clicker::r::right_inventory); ImGui::SameLine(); ImGui::Text("when mouse visible");
            ImGui::Checkbox("##Only Inventory right", &clicker::r::right_only_inventory); ImGui::SameLine(); ImGui::Text("only when mouse visible");
            if (!clicker::r::right_inventory && clicker::r::right_only_inventory) clicker::r::right_inventory = true;

            ImGui::PopStyleVar();
            ImGui::EndChild();
        }
        ImGui::EndChild();

        ImGui::SameLine();

        //CLICKSOUNDS & DOUBLECLICKER
        static int clickertabmisc = 0;
        static int clickertabmisc2 = 0;
        ImGui::SetCursorPosX(ImGui::GetWindowSize().x - ImVec2(ImGui::GetWindowSize().x / 2 - 30, 200).x - 20);
        ImGui::BeginChild("##ClickerExtraOptions", ImVec2(ImGui::GetWindowSize().x / 2 - 30, 300), true);
        
        if (ImGui::BeginTabBar("##ClickerOthers"))
        {
            if (clickertabmisc2 != 0) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.45, 0.45, 0.45, 1));
            if (ImGui::BeginTabItem("clicksounds")) { clickertabmisc = 0; ImGui::EndTabItem(); }
            if (clickertabmisc2 != 0) ImGui::PopStyleColor();

            if (clickertabmisc2 != 1) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.45, 0.45, 0.45, 1));
            if (ImGui::BeginTabItem("double clicker")) { clickertabmisc = 1; ImGui::EndTabItem(); }
            if (clickertabmisc2 != 1) ImGui::PopStyleColor();
            clickertabmisc2 = clickertabmisc;

            ImGui::EndTabBar();
        }

        ImGui::Separator();

        // click sounds
        if (clickertabmisc == 0)
        {
            if (clicksounds::selected_clicksounds.empty())
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
            }
            if (ImGui::Checkbox("enabled", &clicksounds::enabled)) {
                if (clicksounds::enabled) p_SoundPlayer->StartThread();
                else p_SoundPlayer->StopThread();
            }
            if (clicksounds::selected_clicksounds.empty())
            {
                ImGui::PopItemFlag();
                ImGui::PopStyleVar();
            }

            ImGui::Checkbox("custom output device", &clicksounds::use_custom_output);

            ImGui::Text("selected sounds: (%i)", clicksounds::selected_clicksounds.size()); 

            ImGui::BeginChild("##SelectedSoundsList", ImVec2(ImGui::GetWindowSize().x - 15, 80), true);

            if (!clicksounds::selected_clicksounds.empty())
            {
                for (unsigned int i = 0; i < clicksounds::selected_clicksounds.size(); i++)
                {
                    ImGui::Text("[%i] %s", i, clicksounds::selected_clicksounds[i].c_str());

                    ImGui::SameLine();

                    ImGui::PushID(i);
                    if (ImGui::Button("X"))
                    {
                        clicksounds::selected_clicksounds.erase(clicksounds::selected_clicksounds.begin() + i);
                        p_SoundPlayer->GetAllCompatibleSounds(clicksounds::sounds_list, clicksounds::selected_clicksounds);
                    }
                    ImGui::PopID();
                }
            }
            else
            {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "no sounds selected");
                if (clicksounds::enabled)
                {
                    clicksounds::enabled = false;
                    p_SoundPlayer->StopThread();
                }
            }

            ImGui::Separator();

            if (ImGui::Button("refresh"))
            {
                p_SoundPlayer->GetAllCompatibleSounds(clicksounds::sounds_list, clicksounds::selected_clicksounds);
            }
            if (clicksounds::sounds_list.empty())
            {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "no files found");
            }
            else
            {
				static int selected_item = 0;

                for (size_t i = 0; i < clicksounds::sounds_list.size(); i++) {
                    const bool is_selected = (selected_item == i);
                    if (ImGui::Selectable(clicksounds::sounds_list[i].c_str(), is_selected))
                    {
                        selected_item = i;
                    }
                    if (ImGui::IsItemFocused() && ImGui::IsMouseDoubleClicked(0))
                    {
                        std::wstringstream ws;
                        ws << clicksounds::sounds_list[i].c_str();
                        clicksounds::selected_clicksounds.emplace_back(clicksounds::sounds_list[i]);
                        p_SoundPlayer->GetAllCompatibleSounds(clicksounds::sounds_list, clicksounds::selected_clicksounds);
                    }
                }
            }
            
            ImGui::EndChild(); //clicksounds 

            if (!clicksounds::use_custom_output)
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
            }
            
            if (ImGui::BeginCombo("##OutputDevices", clicksounds::selected_device.c_str()))
            {
                for (int i = 0; i < clicksounds::audio_device_list.size(); i++)
                {
                    bool selected = (clicksounds::selected_device == clicksounds::audio_device_list[i]);
                    if (ImGui::Selectable(clicksounds::audio_device_list[i].c_str(), selected))
                    {
                        clicksounds::selected_device = clicksounds::audio_device_list[i];
                        clicksounds::selected_device_ID = i;
                    //  p_SoundPlayer->get_audioDevVol(&clicksounds::volume);
                    }
                    if (selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::Checkbox("randomize volume", &clicksounds::randomize_volume);

            if (clicksounds::randomize_volume)
            {
                ImGui::Text("min");
                if (ImGui::SliderInt("##minVol", &clicksounds::volume_min, 5, 50, ""))
                {
                    if (clicksounds::volume_min > clicksounds::volume_max)
                    {
                        clicksounds::volume_max = clicksounds::volume_min;
                    }
                }
                ImGui::Text("max");
                if (ImGui::SliderInt("##maxVol", &clicksounds::volume_max, 5, 50, ""))
                {
                    if (clicksounds::volume_max < clicksounds::volume_min)
                    {
                        clicksounds::volume_min = clicksounds::volume_max;
                    }
                }
            }
            else
            {
                ImGui::SliderInt("##volSlider", &clicksounds::volume_percent, 5, 50, "");
            }

            if (!clicksounds::use_custom_output)
            {
                ImGui::PopItemFlag();
                ImGui::PopStyleVar();
            }

            //temp
#ifdef _DEBUG
            if (ImGui::Button("Test"))
                p_SoundPlayer->TriggerSoundPlay();
#endif
        }

        // double clicker
        else
        {
            if (ImGui::Checkbox("enabled", &double_clicker::enabled))
            {
                if (double_clicker::enabled)
                    p_doubleClicker->StartThread();
                else
                    p_doubleClicker->StopThread();
            }
            ImGui::SameLine(); ImGui::TextColored(ImColor(51, 51, 51), "[%s]", &double_clicker::key);
            if (ImGui::IsItemClicked()) { double_clicker::key = ".."; binding = true; }
            
            ImGui::Text("delay");
            ImGui::SliderInt("##delay", &double_clicker::delay, 0, 200, "%dms");
            ImGui::Spacing();

            ImGui::Text("chance");
            ImGui::SliderInt("##chance", &double_clicker::chance, 1, 100, "%d%%");
            ImGui::Spacing();

            ImGui::Text("min interval");
            if (ImGui::SliderInt("##min interval", &double_clicker::min_interval, 1, 15, "%dms"))
            {
                if (double_clicker::min_interval > double_clicker::max_interval)
                {
                    double_clicker::max_interval = double_clicker::min_interval;
                }
            }
            ImGui::Spacing();

            ImGui::Text("max interval");
            if (ImGui::SliderInt("##max interval", &double_clicker::max_interval, 1, 15, "%dms"))
            {
                if (double_clicker::max_interval < double_clicker::min_interval)
                {
                    double_clicker::min_interval = double_clicker::max_interval;
                }
            }

            if (double_clicker::min_interval > double_clicker::max_interval) double_clicker::max_interval = double_clicker::min_interval;
        }
        ImGui::EndChild();
    }
  
    //configs
    else if (current_tab == 1) {
        ImGui::BeginChild("##Configs", ImVec2(ImGui::GetWindowSize().x / 2 - 30, 300), true);
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x / 2) - ImGui::CalcTextSize("configs").x + 10);
        ImGui::TextColored(ImColor(122, 122, 122), " configs");
        static char buf[25];

        ImGui::Separator();

        ImGui::SetCursorPosX((ImGui::GetWindowSize().x / 2) - ImGui::CalcTextSize("ServerPresetsConfigs").x + 45);

        ImGui::Spacing();

        ImGui::BeginChild("##ConfigBox", ImVec2(200, 150), true);

        if (!misc::config_list.empty())
        {
            for (size_t i = 0; i < misc::config_list.size(); i++)
            {
                std::call_once(flag, [&]()
                    {
                        misc::selected_config = i;
                        std::strcpy(buf, misc::config_list[i].c_str());
                    });
                const bool is_selected = (misc::selected_config == i);
                if (ImGui::Selectable(misc::config_list[i].c_str(), is_selected))
                {
                    misc::selected_config = i;
                    std::strcpy(buf, misc::config_list[i].c_str());
                }
            }

        }
        else
        {
            ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "   No configs found,  ");
            ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "    insert a name     ");
            ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "   and press Create   ");
            std::call_once(onceFlag, [&]() {
                std::strcpy(buf, "config");
                });
        }

        ImGui::EndChild();

        ImGui::InputText("##SelectedConfig", buf, sizeof(buf) - 1);

        if (ImGui::Button("Refresh"))
        {
            misc::config_list.clear();
            misc::config_list = config::get_all_toad_configs(misc::exe_path);
        }

        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::SetCursorPosX(ImGui::GetWindowSize().x - ImVec2(ImGui::GetWindowSize().x / 2 - 30, 200).x - 20);
        ImGui::BeginChild("##ConfigOptions", ImVec2(ImGui::GetWindowSize().x / 2 - 30, 160), true);
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x / 2) - ImGui::CalcTextSize("options").x + 10);
        ImGui::TextColored(ImColor(122, 122, 122), " options");

        ImGui::Separator();

        if (ImGui::Button("Refresh"))
        {
            misc::config_list.clear();
            misc::config_list = config::get_all_toad_configs(misc::exe_path);
        }
        if (ImGui::Button("Load"))
        {
            std::string s = "\\";
            s.append(buf);
            s.append(".toad");
            config::load_config(misc::exe_path + s);
        }

        // they can select one but still name it an exsisting config
        if (!misc::config_list.empty())
        {
            if (std::strcmp(buf, misc::config_list[misc::selected_config].c_str()) == 0 || std::strcmp(buf, "") == 0)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.05f, 0.05f, 0.05f, 1));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.05f, 0.05f, 0.05f, 1));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.05f, 0.05f, 0.05f, 1));
                ImGui::Button("Create");
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
            }   
            else
            {
                if (ImGui::Button("Create"))
                {
                    std::string s(buf);
                    config::save_config(s);

                    misc::config_list.clear();
                    misc::config_list = config::get_all_toad_configs(misc::exe_path);
                }
            }
        }
        else
        {
            if (std::strcmp(buf, "") == 0)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.05f, 0.05f, 0.05f, 1));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.05f, 0.05f, 0.05f, 1));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.05f, 0.05f, 0.05f, 1));
                ImGui::Button("Create");
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
            }
            else
            {
                if (ImGui::Button("Create"))
                {
                    std::string s(buf);
                    config::save_config(s);

                    misc::config_list.clear();
                    misc::config_list = config::get_all_toad_configs(misc::exe_path);
                }
            }
        }

        if (ImGui::Button("Save"))
        {
            std::string s = "\\";
            s.append(buf);
            config::save_config(misc::exe_path + s);
        }

        ImGui::EndChild();
    }

    //recorder
    else if (current_tab == 2)
    {
        static char buf[25];
        static char buf2[15] = ".txt";

        ImGui::BeginChild("##recorderSettings", ImVec2(ImGui::GetWindowSize().x / 2 - 30, 160), true);
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x / 2) - ImGui::CalcTextSize("settings").x + 10);
        ImGui::TextColored(ImColor(122, 122, 122), "  settings");

        ImGui::Separator();

        ImGui::Text("Bound to:");
        ImGui::SameLine(); ImGui::TextColored(ImColor(51, 51, 51), "[%s]", &clickrecorder::key);
        if (ImGui::IsItemClicked()) {
            clickrecorder::key = ".."; 
            binding = true; 
        }
        
        ImGui::Checkbox("unbind on toggle off", &clickrecorder::auto_unbind);
        ImGui::Checkbox("custom file extension", &clickrecorder::custom_extension);
        ImGui::Checkbox("skip delay after time", &clickrecorder::skip_on_delay);
        if (clickrecorder::skip_on_delay)
        {
            std::string frmt;
            clickrecorder::skip_delay == (int)clickrecorder::skip_delay ? frmt = "%.0f" : frmt = "%.1f";
            ImGui::PushItemWidth(100);
            ImGui::InputDouble("##skipDelay", &clickrecorder::skip_delay, 0.5, 1, frmt.c_str());
            ImGui::PopItemWidth();
            ImGui::SameLine();
            clickrecorder::skip_delay == 1.0 ? ImGui::Text("second") : ImGui::Text("seconds");
            if (clickrecorder::skip_delay < 0.1) clickrecorder::skip_delay = 0.1;
        }
        ImGui::PushItemWidth(120);
        ImGui::InputText("##name", buf, 25);
        ImGui::PopItemWidth();
        if (clickrecorder::custom_extension) {
            if (buf2[0] != '.') buf2[0] = '.';
            ImGui::SameLine();
            ImGui::SetCursorPosX(130);
            ImGui::PushItemWidth(ImGui::CalcTextSize(buf2).x + 5);
            ImGui::InputText("##extension", buf2, 15, ImGuiInputTextFlags_NoCursorFocus);
            ImGui::PopItemWidth();
        }
        else
        {
            ImGui::SameLine();
            ImGui::SetCursorPosX(130);
            ImGui::PushItemWidth(ImGui::CalcTextSize(".txt").x + 5);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.1f, 0.1f, 1));
            ImGui::InputText("##defaultext", ".txt", 5, ImGuiInputTextFlags_ReadOnly);
            ImGui::PopStyleColor();
            ImGui::PopItemWidth();
        }
        if (ImGui::Button("load file"))
        {
			clickrecorder::custom_extension ? p_clickRecorder->LoadFile(buf, buf2) : p_clickRecorder->LoadFile(buf);
        }
        if (!clickrecorder::enabled && !clickrecorder::click_delays.empty())
        {
            ImGui::SameLine();
            if (ImGui::Button("save file"))
            {
                clickrecorder::custom_extension ? p_clickRecorder->SaveFile(buf, buf2) : p_clickRecorder->SaveFile(buf);
            }
        }
        
        ImGui::EndChild(); // end of recorderSettings child
        
        ImGui::SameLine();

        ImGui::SetCursorPosX(ImGui::GetWindowSize().x - ImVec2(ImGui::GetWindowSize().x / 2 - 30, 200).x - 20);
        ImGui::BeginChild("##recorderInfo", ImVec2(ImGui::GetWindowSize().x / 2 - 30, 160),true);
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x / 2) - ImGui::CalcTextSize("info").x + 10);
        ImGui::TextColored(ImColor(122, 122, 122), "info");

        ImGui::Separator();
        ImGui::Text("status: ");

        ImGui::SameLine();

        switch (clickrecorder::record_status)
        {
        case clickrecorder::recordStatus::AWAITING_FOR_CLICK:
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "awaiting first click");
            break;
        case clickrecorder::recordStatus::NOT_RECORDING:
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "not recording");
            break;
        case clickrecorder::recordStatus::RECORDING:
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "recording");
            break;
        case clickrecorder::recordStatus::SKIPPING_NEXT_CLICK:
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "skipping click");
            break;
        }
        
        ImGui::Text("average cps: [%.2f]", clickrecorder::average_cps);
        ImGui::Text("total clicks: [%i]", clickrecorder::total_clicks);
        ImGui::Text("lines loaded: [%i]", clickrecorder::click_delays.size());

        //ImGui::Text("Starting point: %i", );
        
        ImGui::PushItemWidth(50);
        if (clickrecorder::click_delays.empty())
        {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha / 2);
        }
        ImGui::Text("multiplier");
        ImGui::SameLine();
        if (ImGui::DragFloat("##ClickRecordermultiplier", &clickrecorder::multiplier, 0, 1, 2, "%.2f", 1) && !clickrecorder::click_delays.empty())
            p_clickRecorder->CalcVars();
        if (clickrecorder::click_delays.empty())
        {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }
        ImGui::PopItemWidth();
        ImGui::EndChild(); // end of recorderInfo child

        ImGui::SetCursorPos(ImVec2(20, 230));
        ImGui::BeginChild("##recorderPlayback", ImVec2(ImGui::GetWindowSize().x - 40, 130), true);
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x / 2) - ImGui::CalcTextSize("playback").x + 10);
        ImGui::TextColored(ImColor(122, 122, 122), "  playback");

        ImGui::Separator();
        
        if (ImGui::Checkbox("enabled", &clickrecorder::playback_enabled))
        {
            if (clickrecorder::playback_enabled)
                p_clickRecorder->StartPlaybackThread();
            else
				p_clickRecorder->StopPlaybackThread();
        }
        ImGui::SameLine(); ImGui::TextColored(ImColor(51, 51, 51), "[%s]", &clickrecorder::key_playback);
        if (ImGui::IsItemClicked()) { clickrecorder::key_playback = ".."; binding = true; }
        if (clickrecorder::click_delays.empty()) { ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 0, 0, 1), "There are no clicks loaded"); }
        
        ImGui::PushID("recorderwhenvisib");
        ImGui::Checkbox("when mouse visible", &clickrecorder::inventory);
        ImGui::PopID();
        ImGui::Checkbox("randomize start", &clickrecorder::randomize_start_point);
        ImGui::Text("starting point"); ImGui::SameLine();
        ImGui::InputInt("##Playbackstartpoint", &clickrecorder::click_start_point, 2);
        if (clickrecorder::click_start_point < 0 || clickrecorder::click_start_point > clickrecorder::click_delays.size()) clickrecorder::click_start_point = 0;
        if (!clickrecorder::is_start_point_valid()) { clickrecorder::click_start_point++; }
        ImGui::EndChild(); // end of recorderPlayback child
    }

    //misc
    else if (current_tab == 3)
    {
        ImGui::BeginChild("misc", ImVec2(ImGui::GetWindowSize().x / 2 - 30, 300), true);
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x / 2) - ImGui::CalcTextSize("misc").x + 10);
        ImGui::TextColored(ImColor(122, 122, 122), "misc");

        ImGui::Separator();

        ImGui::Checkbox("beep on toggle", &misc::beep_on_toggle);
        if (ImGui::Checkbox("compatibility mode", &misc::compatibility_mode))
            if (misc::compatibility_mode) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
            }
            else 
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
            }

        if (ImGui::Checkbox("use mouse_event", &misc::use_mouse_event));
       
        ImGui::Text("hide bind"); ImGui::SameLine(); ImGui::TextColored(ImColor(51, 51, 51), "[%s]", &misc::hide_key);
        if (ImGui::IsItemClicked()) { misc::hide_key = ".."; binding = true; }

        ImGui::ColorPicker3("##GuiCol", theme::main_col);

        if (ImGui::Checkbox("loop hue color", &theme::hue_loop_mode))
            if (!theme::hue_loop_mode)
            {
                theme::main_col[0] = previousColor[0];
                theme::main_col[1] = previousColor[1];
                theme::main_col[2] = previousColor[2];
                theme::main_col[3] = previousColor[3];
            }
            else
            {
                previousColor[0] = theme::main_col[0];
                previousColor[1] = theme::main_col[1];
                previousColor[2] = theme::main_col[2];
                previousColor[3] = theme::main_col[3];
            }
        if (theme::hue_loop_mode)
            ImGui::SliderFloat("##gay amount", &theme::speed, 0.01, 1);

        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::SetCursorPosX(ImGui::GetWindowSize().x - ImVec2(ImGui::GetWindowSize().x / 2 - 30, 200).x - 20);
        ImGui::BeginChild("jitter", ImVec2(ImGui::GetWindowSize().x / 2 - 30, 160), true);
        ImGui::SetCursorPosX(ImGui::GetWindowSize().x / 2 - ImGui::CalcTextSize("jitter").x + 16);
        ImGui::TextColored(ImColor(122, 122, 122), "jitter");

        ImGui::Separator();

        if (ImGui::Checkbox("enable", &jitter::enabled))
        {
            if (jitter::enabled)
                p_Jitter->StartThread();
            else
                p_Jitter->StopThread();
        }
        /*ImGui::Text("intensity x");
        ImGui::SliderInt("##intensity x", &jitter::intensity_X, 1, 10);
        ImGui::Spacing();
        ImGui::Text("intensity y");
        ImGui::SliderInt("##Intensity y", &jitter::intensity_Y, 1, 10);*/
        ImGui::Text("intensity X");
        ImGui::SliderInt("##intensityx", &jitter::intensity_X, 1, 10);
        ImGui::Text("intensity Y");
        ImGui::SliderInt("##intensityy", &jitter::intensity_Y, 1, 10);
        ImGui::Spacing();
        ImGui::Text("chance");
        ImGui::SliderInt("##Chance", &jitter::chance, 1, 100, "%d%%");

        ImGui::EndChild();

        ImGui::SetCursorPosX(ImGui::GetWindowSize().x - ImVec2(ImGui::GetWindowSize().x / 2 - 30, 200).x - 20);
        ImGui::SetCursorPosY(230);
        ImGui::BeginChild("custom window",ImVec2(ImGui::GetWindowSize().x / 2 - 30, 130), true);
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x / 2) - ImGui::CalcTextSize("custom window").x + 10);
        ImGui::TextColored(ImColor(122, 122, 122), "    custom window");

        ImGui::Separator();

        ImGui::Combo("##ClickingWindow", &misc::selected_click_window, misc::window_options, IM_ARRAYSIZE(misc::window_options));

        if (misc::selected_click_window == 2)
        {
            ImGui::InputText("##CustomWindowName", misc::custom_window_title, 50, ImGuiInputTextFlags_ReadOnly);
            ImGui::SameLine();
            if (ImGui::Button("..."))
            {
                misc::proc_list.clear();
                misc::get_window_list();
                showProcessList = !showProcessList;
            }
        }

        if (clicking_window == NULL)
            ImGui::TextColored(ImVec4(205, 0, 0, 255), "no active window found");
        else 
            ImGui::TextColored(ImVec4(0, 205, 0, 255), "Clicking on PID: %d", misc::pid);

        ImGui::EndChild();
    }

    ImGui::SetCursorPosY(ImGui::GetWindowSize().y - 20);
    ImGui::TextColored(ImColor(51, 51, 51), "made by vierkant");

    if (showProcessList && misc::selected_click_window == 2)
    {
        ImGui::Begin("Process List", &showProcessList, ImGuiWindowFlags_NoDocking);
        if (ImGui::Button("refresh"))
        {
            misc::proc_list.clear();
            misc::get_window_list();
        }

		static int selected_item = 0;

        for (size_t i = 0; i < misc::proc_list.size(); i++) 
        {
            const bool is_selected = (selected_item == i);
            if (ImGui::Selectable(misc::proc_list[i].proc_name.c_str(), is_selected))
            {
                selected_item = i;
            }
            if (ImGui::IsItemFocused() && ImGui::IsMouseDoubleClicked(0))
            {
                std::strcpy(misc::custom_window_title, misc::proc_list[i].proc_name.c_str());
                misc::pid = misc::proc_list[i].proc_id;
                clicking_window = misc::proc_list[i].hwnd;
                showProcessList = false;
            }
            if (is_selected)
            {
                if (ImGui::Button("select as active"))
                {
                    std::strcpy(misc::custom_window_title, misc::proc_list[i].proc_name.c_str());
                    misc::pid = misc::proc_list[i].proc_id;
                    clicking_window = misc::proc_list[i].hwnd;
                    showProcessList = false;
                }
            }
        }
        ImGui::End();
    }

    ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 45);
    ImGui::SetCursorPosY(10);

    if (ImGui::Button("exit"))
    {
        is_running = false;
    }

    ImGui::End();
}

void ui_decorations()
{
	ImGuiStyle* style = &ImGui::GetStyle();

	ImGui::ColorConvertRGBtoHSV(theme::main_col[0], theme::main_col[1], theme::main_col[2], h, s, v);

	ImGui::ColorConvertHSVtoRGB(h, s, v - 0.1f, theme::main_col_dark[0], theme::main_col_dark[1], theme::main_col_dark[2]);
	ImGui::ColorConvertHSVtoRGB(h, s, v - 0.2f, theme::main_col_darker[0], theme::main_col_darker[1], theme::main_col_darker[2]);
	ImGui::ColorConvertHSVtoRGB(h, s, v + 0.1f, theme::main_col_light[0], theme::main_col_light[1], theme::main_col_light[2]);

	style->FramePadding.y = 2.5f;
	style->TabRounding = 0;

	style->Colors[ImGuiCol_Text] = ImColor(188, 188, 188);

	style->Colors[ImGuiCol_ChildBg] = ImColor(20, 20, 20);
	style->Colors[ImGuiCol_Border] = ImColor(36, 36, 36);

	style->Colors[ImGuiCol_CheckMark] = ImVec4(theme::main_col[0], theme::main_col[1], theme::main_col[2], 1.f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(theme::main_col[0], theme::main_col[1], theme::main_col[2], 1.f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImColor(0, 60, 12);

	style->Colors[ImGuiCol_Button] = ImVec4(theme::main_col[0], theme::main_col[1], theme::main_col[2], 1.f);

	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(theme::main_col_dark[0], theme::main_col_dark[1], theme::main_col_dark[2], 1.f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(theme::main_col_darker[0], theme::main_col_darker[1], theme::main_col_darker[2], 1.f - 0.20f);

	style->Colors[ImGuiCol_FrameBg] = ImColor(16, 16, 16);
	style->Colors[ImGuiCol_FrameBgActive] = ImColor(16, 16, 16);
	style->Colors[ImGuiCol_FrameBgHovered] = ImColor(16, 16, 16);

	style->Colors[ImGuiCol_PopupBg] = ImColor(16, 16, 16);
	style->Colors[ImGuiCol_HeaderHovered] = ImColor(0, 0, 0, 0);

	style->Colors[ImGuiCol_Tab] = ImColor(36, 36, 36);
	style->Colors[ImGuiCol_TabHovered] = ImVec4(theme::main_col_light[0], theme::main_col_light[1], theme::main_col_light[2], 1);

	style->Colors[ImGuiCol_TabActive] = ImVec4(theme::main_col[0], theme::main_col[1], theme::main_col[2], 1.f);

	style->Colors[ImGuiCol_Header] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);

	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);

}

}