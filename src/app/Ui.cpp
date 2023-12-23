#include "pch.h"
#include "Application.h"
#include "toad.h"

#include "imgui.h"
#include "imgui_internal.h"

#include "UiHelpers.h"

ImDrawList* draw;

int tab                 = 0;
int configTabBar        = 0;
int k                   = 0;
int selected_item       = 0;

bool showProcessList    = false;
bool binding            = false;

std::once_flag flag;
std::once_flag onceFlag, onceFlag2;
float h = 0, s = 0, v = 0;

float rgbModeVal = 0;
float previousColor[4] = {0,0,0,0};

void toad::theme::do_hue_loop(float deltatime)
{
    rgbModeVal += speed * deltatime;
    if (rgbModeVal >= 1)
        rgbModeVal = 0;
    ImGui::ColorConvertHSVtoRGB(rgbModeVal, s, v, toad::theme::main_col[0], toad::theme::main_col[1], toad::theme::main_col[2]);
    
}

void decorations() {
    ImGuiStyle* style = &ImGui::GetStyle();
    
    ImGui::ColorConvertRGBtoHSV(toad::theme::main_col[0], toad::theme::main_col[1], toad::theme::main_col[2], h, s, v);
    
    ImGui::ColorConvertHSVtoRGB(h, s, v - 0.1f, toad::theme::main_col_dark[0], toad::theme::main_col_dark[1], toad::theme::main_col_dark[2]);
    ImGui::ColorConvertHSVtoRGB(h, s, v - 0.2f, toad::theme::main_col_darker[0], toad::theme::main_col_darker[1], toad::theme::main_col_darker[2]);
    ImGui::ColorConvertHSVtoRGB(h, s, v + 0.1f, toad::theme::main_col_light[0], toad::theme::main_col_light[1], toad::theme::main_col_light[2]);

    style->FramePadding.y = 2.5f;
    style->TabRounding = 0;

    style->Colors[ImGuiCol_Text] = ImColor(188, 188, 188);

    style->Colors[ImGuiCol_ChildBg] = ImColor(20, 20, 20);
    style->Colors[ImGuiCol_Border] = ImColor(36, 36, 36);

    style->Colors[ImGuiCol_CheckMark] = ImVec4(toad::theme::main_col[0], toad::theme::main_col[1], toad::theme::main_col[2], 1.f);
    style->Colors[ImGuiCol_SliderGrab] = ImVec4(toad::theme::main_col[0], toad::theme::main_col[1], toad::theme::main_col[2], 1.f);
    style->Colors[ImGuiCol_SliderGrabActive] = ImColor(0, 60, 12);
    
    style->Colors[ImGuiCol_Button] = ImVec4(toad::theme::main_col[0], toad::theme::main_col[1], toad::theme::main_col[2], 1.f);
   
    style->Colors[ImGuiCol_ButtonHovered] = ImVec4(toad::theme::main_col_dark[0], toad::theme::main_col_dark[1], toad::theme::main_col_dark[2], 1.f);
    style->Colors[ImGuiCol_ButtonActive] = ImVec4(toad::theme::main_col_darker[0], toad::theme::main_col_darker[1], toad::theme::main_col_darker[2], 1.f - 0.20f);

    style->Colors[ImGuiCol_FrameBg] = ImColor(16, 16, 16);
    style->Colors[ImGuiCol_FrameBgActive] = ImColor(16, 16, 16);
    style->Colors[ImGuiCol_FrameBgHovered] = ImColor(16, 16, 16);

    style->Colors[ImGuiCol_PopupBg] = ImColor(16, 16, 16);
    style->Colors[ImGuiCol_HeaderHovered] = ImColor(0, 0, 0, 0);

    style->Colors[ImGuiCol_Tab] = ImColor(36, 36, 36);
    style->Colors[ImGuiCol_TabHovered] = ImVec4(toad::theme::main_col_light[0], toad::theme::main_col_light[1], toad::theme::main_col_light[2], 1);

    style->Colors[ImGuiCol_TabActive] = ImVec4(toad::theme::main_col[0], toad::theme::main_col[1], toad::theme::main_col[2], 1.f);

    style->Colors[ImGuiCol_Header] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);

	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);

}

// handles all hotkey presses and binding something to a key
void toad::hotkey_handler() {
    if (!binding) {
        //misc Hide and Unhide
        if ((GetAsyncKeyState(toad::misc::keycode) & 1) && (k == 0))
        {
            //if (toad::misc::window_hidden) { toad::misc::show(window); toad::misc::window_hidden = false; }
            //else if (!toad::misc::window_hidden) { toad::misc::hide(window); toad::misc::window_hidden = true; }
            k = 1;
        }
        else if (GetAsyncKeyState(toad::misc::keycode) == 0) k = 0;

        //Clicker L&R
        switch (toad::clicker::selected_enable_option)
        {
        case 2:
        case 0:
            if (GetAsyncKeyState(toad::clicker::keycode) & 1) {
                if (toad::clicker::keycode == toad::clicker::r::keycode)
                {
                    if (toad::clicker::r::enabled) p_right_clicker->stop_thread();
                    else p_right_clicker->start_thread();
                    toad::clicker::r::enabled = !toad::clicker::r::enabled;
                }
                if (toad::misc::beep_on_toggle) Beep(350, 100);
                if (toad::clicker::enabled) p_clicker->stop_thread();
                else p_clicker->start_thread();
                toad::clicker::enabled = !toad::clicker::enabled;
            }
            break;
        default:
            break;
        }
        switch (toad::clicker::r::selected_enable_option)
        {
        case 2:
        case 0:
            if (GetAsyncKeyState(toad::clicker::r::keycode) & 1) {
                if (toad::misc::beep_on_toggle) Beep(350, 100);
                if (toad::clicker::r::enabled) p_right_clicker->stop_thread();
                else p_right_clicker->start_thread();
                toad::clicker::r::enabled = !toad::clicker::r::enabled;
            }
            break;
        default:
            break;
        }  

        //click recorder 
        if (GetAsyncKeyState(toad::clickrecorder::keycode) & 1) {
            if (toad::clickrecorder::enabled)
            {
                ClickRecorder::get().stop_record_thread();

                if (toad::clickrecorder::auto_unbind) // unbind on stopping recording
                {
                    toad::clickrecorder::keycode = 0;
                    toad::clickrecorder::key = "none";
                }
                toad::clickrecorder::click_start_point = 0;
                toad::clickrecorder::record_status = RECORDSTATUS::NOT_RECORDING;
            }
            else
            {
                ClickRecorder::get().start_record_thread();
                toad::clickrecorder::total_clicks = 0;
                toad::clickrecorder::record_status = RECORDSTATUS::AWAITING_FOR_CLICK;
                ClickRecorder::get().reset();
            }

            toad::clickrecorder::enabled = !toad::clickrecorder::enabled;
        }

        //click playback
        if (GetAsyncKeyState(toad::clickrecorder::keycode_playback) & 1 && !toad::clickrecorder::click_delays.empty())
        {
            if (!toad::clickrecorder::playback_enabled) 
            {
                ClickRecorder::get().start_playback_thread();
            }
            else
            {
                ClickRecorder::get().stop_playback_thread();
            }

            toad::clickrecorder::playback_enabled = !toad::clickrecorder::playback_enabled;
        }

        if (GetAsyncKeyState(toad::double_clicker::keycode) & 1)
        {
            toad::double_clicker::enabled = !toad::double_clicker::enabled;

            if (toad::double_clicker::enabled)
            {
                LOGDEBUG("starting thread");
                p_doubleClicker->start_thread();
            }
            else
            {
                LOGDEBUG("stopping thread");
                p_doubleClicker->stop_thread();
            }
        }
    }

    // when binding to a button
    else if (binding) {
        // (1)lmb - (123)f12
        for (int i = 3; i < 251; i++) {
            if (toad::clicker::key_str == "..") {
                if (GetAsyncKeyState(i) & 0x8000) {
                    if (i == VK_ESCAPE) { toad::clicker::key_str = "none"; binding = false; toad::clicker::keycode = 0; }
                    else { toad::clicker::key_str = toad::keys[i - 1]; toad::clicker::keycode = i; }
                }
                if (toad::clicker::key_str != "..") binding = false;
            }
            else if (toad::clicker::r::key_str == "..") {
                if (GetAsyncKeyState(i) & 0x8000) {
                    if (i == VK_ESCAPE) { toad::clicker::r::key_str = "none"; binding = false; toad::clicker::r::keycode = 0; }
                    else { toad::clicker::r::key_str = toad::keys[i - 1]; toad::clicker::r::keycode = i; }
                }
                if (toad::clicker::r::key_str != "..") binding = false;
            }
            else if (toad::misc::hide_key == "..") {
                if (GetAsyncKeyState(i) & 0x8000) {
                    if (i == VK_ESCAPE) { toad::misc::hide_key = "none"; binding = false; toad::misc::keycode = 0; }
                    else { toad::misc::hide_key = toad::keys[i - 1]; toad::misc::keycode = i; }
                }
                if (toad::misc::hide_key != "..") binding = false;
            }
            else if (toad::clickrecorder::key == "..") {
                if (GetAsyncKeyState(i) & 0x8000) {
                    if (i == VK_ESCAPE) { toad::clickrecorder::key = "none"; binding = false; toad::clickrecorder::keycode = 0; }
                    else { toad::clickrecorder::key = toad::keys[i - 1]; toad::clickrecorder::keycode = i; }
                }
                if (toad::clickrecorder::key != "..") binding = false;
            }
            else if (toad::clickrecorder::key_playback == "..") {
                if (GetAsyncKeyState(i) & 0x8000) {
                    if (i == VK_ESCAPE) { toad::clickrecorder::key_playback = "none"; binding = false; toad::clickrecorder::keycode_playback = 0; }
                    else { toad::clickrecorder::key_playback = toad::keys[i - 1]; toad::clickrecorder::keycode_playback = i; }
                }
                if (toad::clickrecorder::key_playback != "..") binding = false;
            }
            else if (toad::double_clicker::key == "..") {
                if (GetAsyncKeyState(i) & 0x8000) {
                    if (i == VK_ESCAPE) { toad::double_clicker::key = "none"; binding = false; toad::double_clicker::keycode = 0; }
                    else { toad::double_clicker::key = toad::keys[i - 1]; toad::double_clicker::keycode = i; }
                }
                if (toad::double_clicker::key != "..") binding = false;
            }
        }
    }
}

void toad::render_ui(ImGuiIO* io) {

    toad::hotkey_handler();

    ImGui::SetNextWindowSize(io->DisplaySize);
    ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->Pos.x,ImGui::GetMainViewport()->Pos.y });
    ImGui::Begin("Toad", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoNavInputs);

    decorations();
    if (toad::theme::hue_loop_mode)
        toad::theme::do_hue_loop(io->DeltaTime);

   /* ImGui::TextColored(ImVec4(toad::theme::main_col[0], toad::theme::main_col[1], toad::theme::main_col[2], 1.f), "toad");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1), "minecraft");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1), toad::APP_VER);
#ifdef _DEBUG
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1), "Debug Mode");
#endif*/
    if (ImGui::BeginTabBar("##tabbar"))
    {
        if (ImGui::BeginTabItem("  clicker  ", nullptr))
        {
            tab = 0;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("  configs  ", nullptr))
        {
            if (toad::config::ConfigList.empty())
                toad::config::ConfigList = toad::config::GetAllToadConfigs(toad::misc::exe_path);

            else
            {
                toad::config::ConfigList.clear();
                toad::config::ConfigList = toad::config::GetAllToadConfigs(toad::misc::exe_path);
            }

            tab = 1;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("  recorder  ", nullptr))
        {
            tab = 2;
            ImGui::EndTabItem();
        } 
        if (ImGui::BeginTabItem("   misc    ", nullptr))
        {
            tab = 3;
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::SetCursorPosX(20);
    ImGui::SetCursorPosY(60);

    //clicker
    if (tab == 0) {

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

            if (toad::CheckBoxFill("##Enable Left", &toad::clicker::enabled))
            {
                if (toad::clicker::enabled)
                    p_clicker->start_thread();
                else p_clicker->stop_thread();
            }

            ImGui::SameLine();
            ImGui::Text("enable");
            ImGui::SameLine();
            ImGui::TextColored(ImColor(51, 51, 51), "[%s]", &toad::clicker::key_str);

            if (ImGui::IsItemClicked()) { toad::clicker::key_str = ".."; binding = true; }
            
            ImGui::Combo("##EnableOptions", &toad::clicker::selected_enable_option, toad::clicker::enable_options, IM_ARRAYSIZE(toad::clicker::enable_options));
            ImGui::Spacing();

            if (toad::misc::use_mouse_event && toad::clicker::selected_enable_option == 0) toad::clicker::selected_enable_option = 1;
            
            if (toad::clicker::one_slider)
            {
                ImGui::Text("average cps");
                ImGui::SliderInt("##cps", &toad::clicker::cps, 5, 20, "%dcps");
            }
            else
            {
                ImGui::Text("min");
                if (toad::clicker::blatant_mode) ImGui::SliderInt("##Min", &toad::clicker::min_cps, 5, 100, "%dcps");
                else ImGui::SliderInt("##Min", &toad::clicker::min_cps, 5, 20, "%dcps");

                ImGui::Text("max");

                if (toad::clicker::blatant_mode) ImGui::SliderInt("##Max", &toad::clicker::max_cps, 5, 100, "%dcps");
                else ImGui::SliderInt("##Max", &toad::clicker::max_cps, 5, 20, "%dcps");
            }

            ImGui::Spacing();

            toad::CheckBoxFill("rmb lock", &toad::clicker::rmb_lock);
            toad::CheckBoxFill("when mouse visible", &toad::clicker::inventory);
            toad::CheckBoxFill("blatant mode", &toad::clicker::blatant_mode);
            if (ImGui::IsItemClicked())
            {
                if (toad::clicker::max_cps > 20) toad::clicker::max_cps = 20;
                if (toad::clicker::min_cps > 20) toad::clicker::min_cps = 20;
            }

            toad::CheckBoxFill("prioritize higher cps", &toad::clicker::higher_cps);
            toad::CheckBoxFill("one slider", &toad::clicker::one_slider);

            if (!toad::options_found)
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha / 2);
            }
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1.f, 0.5f));
            toad::CheckBoxFill("slot whitelist", &toad::clicker::slot_whitelist);
            if (toad::clicker::slot_whitelist) {
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.7f, 1.f));
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.2f, 3.f));
                ImGui::SetCursorPosX(10);
                toad::CheckBoxFill("##slot1", &toad::clicker::whitelisted_slots[0]); ImGui::SameLine();
                toad::CheckBoxFill("##slot2", &toad::clicker::whitelisted_slots[1]); ImGui::SameLine();
                toad::CheckBoxFill("##slot3", &toad::clicker::whitelisted_slots[2]); ImGui::SameLine();
                toad::CheckBoxFill("##slot4", &toad::clicker::whitelisted_slots[3]); ImGui::SameLine();
                toad::CheckBoxFill("##slot5", &toad::clicker::whitelisted_slots[4]); ImGui::SameLine();
                toad::CheckBoxFill("##slot6", &toad::clicker::whitelisted_slots[5]); ImGui::SameLine();
                toad::CheckBoxFill("##slot7", &toad::clicker::whitelisted_slots[6]); ImGui::SameLine();
                toad::CheckBoxFill("##slot8", &toad::clicker::whitelisted_slots[7]); ImGui::SameLine();
                toad::CheckBoxFill("##slot9", &toad::clicker::whitelisted_slots[8]);
                ImGui::PopStyleVar();
                ImGui::PopStyleVar();
            }

            ImGui::PopStyleVar();

            if (!toad::options_found)
            {
                ImGui::PopItemFlag();
                ImGui::PopStyleVar();
            }

            if (toad::clicker::min_cps > toad::clicker::max_cps) toad::clicker::min_cps = toad::clicker::max_cps;
            if (toad::clicker::r::min_cps > toad::clicker::r::max_cps) toad::clicker::r::min_cps = toad::clicker::r::max_cps;

            ImGui::PopStyleVar();

            ImGui::EndChild();
        }
        //RIGHT CLICKER
        else if (clickertab == 1)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 3.5f));

            ImGui::BeginChild("right", ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y - 50));

            ImGui::Separator();

            if (toad::CheckBoxFill("##Enable right", &toad::clicker::r::enabled))
            {
                if (toad::clicker::r::enabled)
                    p_right_clicker->start_thread();
                else p_right_clicker->stop_thread();
            }

            ImGui::SameLine(); 
            ImGui::Text("enable"); 
            ImGui::SameLine();

            ImGui::PushID("rightBindText");
            ImGui::TextColored(ImColor(51, 51, 51), "[%s]", &toad::clicker::r::key_str);
            ImGui::PopID();

            if (ImGui::IsItemClicked()) { toad::clicker::r::key_str = ".."; binding = true; }

            ImGui::Combo("##EnableOptionsRight", &toad::clicker::r::selected_enable_option, toad::clicker::r::enable_options, IM_ARRAYSIZE(toad::clicker::r::enable_options));

            if (toad::misc::use_mouse_event && toad::clicker::r::selected_enable_option == 0) toad::clicker::r::selected_enable_option = 1;

            ImGui::Spacing();

            ImGui::Text("min");
            ImGui::SliderInt("##Min right", &toad::clicker::r::min_cps, 5, 90, "%dcps");
            ImGui::Text("max");
            ImGui::SliderInt("##Max right", &toad::clicker::r::max_cps, 5, 90, "%dcps");

            ImGui::Spacing();

            toad::CheckBoxFill("##Inventory right", &toad::clicker::r::inventory); ImGui::SameLine(); ImGui::Text("when mouse visible");
            toad::CheckBoxFill("##Only Inventory right", &toad::clicker::r::only_inventory); ImGui::SameLine(); ImGui::Text("only when mouse visible");
            if (!toad::clicker::r::inventory && toad::clicker::r::only_inventory) toad::clicker::r::inventory = true;

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
            if (toad::clicksounds::selected_clicksounds.empty())
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
            }
            if (toad::CheckBoxFill("enabled", &toad::clicksounds::enabled)) {
                if (toad::clicksounds::enabled) p_SoundPlayer->start_thread();
                else p_SoundPlayer->stop_thread();
            }
            if (toad::clicksounds::selected_clicksounds.empty())
            {
                ImGui::PopItemFlag();
                ImGui::PopStyleVar();
            }

            toad::CheckBoxFill("custom output device", &toad::clicksounds::use_custom_output);

            ImGui::Text("selected sounds: (%i)", toad::clicksounds::selected_clicksounds.size()); 

            ImGui::BeginChild("##SelectedSoundsList", ImVec2(ImGui::GetWindowSize().x - 15, 80), true);

            if (!toad::clicksounds::selected_clicksounds.empty())
            {
                for (unsigned int i = 0; i < toad::clicksounds::selected_clicksounds.size(); i++)
                {
                    ImGui::Text("[%i] %s", i, toad::clicksounds::selected_clicksounds[i].c_str());

                    ImGui::SameLine();

                    ImGui::PushID(i);
                    if (ImGui::Button("X"))
                    {
                        p_SoundPlayer->ClearCachedFile(toad::misc::exe_path + toad::clicksounds::selected_clicksounds[i].c_str());
                        toad::clicksounds::selected_clicksounds.erase(toad::clicksounds::selected_clicksounds.begin() + i);
                        p_SoundPlayer->GetAllCompatibleSounds(toad::clicksounds::sound_list, toad::clicksounds::selected_clicksounds);
                    }
                    ImGui::PopID();
                }
            }
            else
            {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "no sounds selected");
                if (toad::clicksounds::enabled)
                {
                    toad::clicksounds::enabled = false;
                    p_SoundPlayer->stop_thread();
                }
            }

            ImGui::Separator();

            if (ImGui::Button("refresh"))
            {
                p_SoundPlayer->GetAllCompatibleSounds(toad::clicksounds::sound_list, toad::clicksounds::selected_clicksounds);
            }
            if (toad::clicksounds::sound_list.empty())
            {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "no files found");
            }
            else
            {
                for (size_t i = 0; i < toad::clicksounds::sound_list.size(); i++) {
                    const bool is_selected = (selected_item == i);
                    if (ImGui::Selectable(toad::clicksounds::sound_list[i].c_str(), is_selected))
                    {
                        selected_item = i;
                    }
                    if (ImGui::IsItemFocused() && ImGui::IsMouseDoubleClicked(0))
                    {
                        std::wstringstream ws;
                        ws << toad::clicksounds::sound_list[i].c_str();
                        toad::clicksounds::selected_clicksounds.emplace_back(toad::clicksounds::sound_list[i]);
                        p_SoundPlayer->CacheAudioFiles(toad::clicksounds::selected_clicksounds);
                        p_SoundPlayer->GetAllCompatibleSounds(toad::clicksounds::sound_list, toad::clicksounds::selected_clicksounds);
                    }
                }
            }
            
            ImGui::EndChild(); //clicksounds 

            if (!toad::clicksounds::use_custom_output)
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
            }
            
            if (ImGui::BeginCombo("##OutputDevices", toad::clicksounds::selected_device.c_str()))
            {
                for (int i = 0; i < toad::clicksounds::audio_device_list.size(); i++)
                {
                    bool selected = (toad::clicksounds::selected_device == toad::clicksounds::audio_device_list[i]);
                    if (ImGui::Selectable(toad::clicksounds::audio_device_list[i].c_str(), selected))
                    {
                        toad::clicksounds::selected_device = toad::clicksounds::audio_device_list[i];
                        p_SoundPlayer->SetAudioDevice(i);
                    }
                    if (selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            toad::CheckBoxFill("randomize volume", &toad::clicksounds::randomize_volume);

            if (toad::clicksounds::randomize_volume)
            {
                ImGui::Text("min");
                ImGui::SliderInt("##minVol", &toad::clicksounds::volume_min, 0, 0xFFFF, "");
                ImGui::Text("max");
                ImGui::SliderInt("##maxVol", &toad::clicksounds::volume_max, 0, 0xFFFF, "");
            }
            else
            {
                ImGui::SliderInt("##volSlider", &toad::clicksounds::volume, 0, 0xFFFF, "");
            }

            static float playback_rate_multiplier = 1;
            if (ImGui::SliderFloat("playback rate", &playback_rate_multiplier, 0.1f, 2.f))
            {
                p_SoundPlayer->SetPlayBackRate(playback_rate_multiplier);
            }

            if (!toad::clicksounds::use_custom_output)
            {
                ImGui::PopItemFlag();
                ImGui::PopStyleVar();
            }

            //temp
#ifdef _DEBUG
            if (ImGui::Button("Test"))
                toad::clicksounds::play = true;
#endif
        }

        // double clicker
        else
        {
            if (toad::CheckBoxFill("enabled", &toad::double_clicker::enabled))
            {
                if (toad::double_clicker::enabled)
                    p_doubleClicker->start_thread();
                else
                    p_doubleClicker->stop_thread();
            }
            ImGui::SameLine(); ImGui::TextColored(ImColor(51, 51, 51), "[%s]", &toad::double_clicker::key);
            if (ImGui::IsItemClicked()) { toad::double_clicker::key = ".."; binding = true; }
            
            ImGui::Text("delay");
            ImGui::SliderInt("##delay", &toad::double_clicker::delay, 0, 200, "%dms");
            ImGui::Spacing();

            ImGui::Text("chance");
            ImGui::SliderInt("##chance", &toad::double_clicker::chance, 1, 100, "%d%%");
            ImGui::Spacing();

            ImGui::Text("min interval");
            ImGui::SliderInt("##min interval", &toad::double_clicker::min_interval, 1, 15, "%dms");
            ImGui::Spacing();

            ImGui::Text("max interval");
            ImGui::SliderInt("##max interval", &toad::double_clicker::max_interval, 1, 15, "%dms");

            if (toad::double_clicker::min_interval > toad::double_clicker::max_interval) toad::double_clicker::max_interval = toad::double_clicker::min_interval;
        }
        ImGui::EndChild();
    }
  
    //configs
    else if (tab == 1) {
        ImGui::BeginChild("##Configs", ImVec2(ImGui::GetWindowSize().x / 2 - 30, 300), true);
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x / 2) - ImGui::CalcTextSize("configs").x + 10);
        ImGui::TextColored(ImColor(122, 122, 122), " configs");
        static char buf[25];

        ImGui::Separator();

        ImGui::SetCursorPosX((ImGui::GetWindowSize().x / 2) - ImGui::CalcTextSize("ServerPresetsConfigs").x + 45);

        ImGui::Spacing();

        ImGui::BeginChild("##ConfigBox", ImVec2(200, 150), true);

        if (!toad::config::ConfigList.empty())
        {
            for (size_t i = 0; i < toad::config::ConfigList.size(); i++)
            {
                std::call_once(flag, [&]()
                    {
                        toad::config::selectedConfig = i;
                        std::strcpy(buf, toad::config::ConfigList[i].c_str());
                    });
                const bool is_selected = (toad::config::selectedConfig == i);
                if (ImGui::Selectable(toad::config::ConfigList[i].c_str(), is_selected))
                {
                    toad::config::selectedConfig = i;
                    std::strcpy(buf, toad::config::ConfigList[i].c_str());
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
            toad::config::ConfigList.clear();
            toad::config::ConfigList = toad::config::GetAllToadConfigs(toad::misc::exe_path);
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
            toad::config::ConfigList.clear();
            toad::config::ConfigList = toad::config::GetAllToadConfigs(toad::misc::exe_path);
        }
        if (ImGui::Button("Load"))
        {
            std::string s = "\\";
            s.append(buf);
            s.append(".toad");
            toad::config::loadConfig(toad::misc::exe_path + s);
        }

        // they can select one but still name it an exsisting config
        if (!toad::config::ConfigList.empty())
        {
            if (std::strcmp(buf, toad::config::ConfigList[toad::config::selectedConfig].c_str()) == 0 || std::strcmp(buf, "") == 0)
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
                    toad::config::saveConfig(s);

                    toad::config::ConfigList.clear();
                    toad::config::ConfigList = toad::config::GetAllToadConfigs(toad::misc::exe_path);
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
                    toad::config::saveConfig(s);

                    toad::config::ConfigList.clear();
                    toad::config::ConfigList = toad::config::GetAllToadConfigs(toad::misc::exe_path);
                }
            }
        }

        if (ImGui::Button("Save"))
        {
            std::string s = "\\";
            s.append(buf);
            toad::config::saveConfig(toad::misc::exe_path + s);
        }

        ImGui::EndChild();
    }

    //recorder
    else if (tab == 2)
    {
        static char buf[25];
        static char buf2[15] = ".txt";

        ImGui::BeginChild("##recorderSettings", ImVec2(ImGui::GetWindowSize().x / 2 - 30, 160), true);
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x / 2) - ImGui::CalcTextSize("settings").x + 10);
        ImGui::TextColored(ImColor(122, 122, 122), "  settings");

        ImGui::Separator();

        ImGui::Text("Bound to:");
        ImGui::SameLine(); ImGui::TextColored(ImColor(51, 51, 51), "[%s]", &toad::clickrecorder::key);
        if (ImGui::IsItemClicked()) {
            toad::clickrecorder::key = ".."; 
            binding = true; 
        }
        
        toad::CheckBoxFill("unbind on toggle off", &toad::clickrecorder::auto_unbind);
        toad::CheckBoxFill("custom file extension", &toad::clickrecorder::custom_extension);
        toad::CheckBoxFill("skip delay after time", &toad::clickrecorder::skip_on_delay);

        if (toad::clickrecorder::skip_on_delay)
        {
            char frmt[5];
            toad::clickrecorder::skip_delay_threshold == (int)toad::clickrecorder::skip_delay_threshold ? strncpy(frmt, "%.0f", sizeof frmt) : strncpy(frmt, "%.1f", sizeof frmt);
            ImGui::PushItemWidth(100);
            ImGui::InputDouble("##skipDelay", &toad::clickrecorder::skip_delay_threshold, 0.5, 1, frmt);
            ImGui::PopItemWidth();
            ImGui::SameLine();
            toad::clickrecorder::skip_delay_threshold == 1.0 ? ImGui::Text("second") : ImGui::Text("seconds");
            if (toad::clickrecorder::skip_delay_threshold < 0.1) toad::clickrecorder::skip_delay_threshold = 0.1;
        }
        ImGui::PushItemWidth(120);
        ImGui::InputText("##name", buf, 25);
        ImGui::PopItemWidth();
        if (toad::clickrecorder::custom_extension) {
            if (buf2[0] != '.') buf2[0] = '.';
            ImGui::SameLine();
            ImGui::SetCursorPosX(130);
            ImGui::PushItemWidth(ImGui::CalcTextSize(buf2).x + 5);
            ImGui::InputText("##extension", buf2, 15);
            ImGui::PopItemWidth();
        }
        else
        {
            ImGui::SameLine();
            ImGui::SetCursorPosX(130);
            ImGui::PushItemWidth(ImGui::CalcTextSize(".txt").x + 5);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.1f, 0.1f, 1));
            char preview[5] = ".txt";
            ImGui::InputText("##defaultext", preview, 5, ImGuiInputTextFlags_ReadOnly);
            ImGui::PopStyleColor();
            ImGui::PopItemWidth();
        }
        if (ImGui::Button("load file"))
        {
            toad::clickrecorder::custom_extension ? ClickRecorder::get().load_file(buf, buf2) : ClickRecorder::get().load_file(buf);
        }
        if (!toad::clickrecorder::enabled && !toad::clickrecorder::click_delays.empty())
        {
            ImGui::SameLine();
            if (ImGui::Button("save file"))
            {
                toad::clickrecorder::custom_extension ? ClickRecorder::get().save_file(buf, buf2) : ClickRecorder::get().save_file(buf);
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

        switch (toad::clickrecorder::record_status)
        {
        case RECORDSTATUS::AWAITING_FOR_CLICK:
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "awaiting first click");
            break;
        case RECORDSTATUS::NOT_RECORDING:
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "not recording");
            break;
        case RECORDSTATUS::RECORDING:
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "recording");
            break;
        case RECORDSTATUS::SKIPPING_NEXT_CLICK:
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "skipping click");
            break;
        }
        
        ImGui::Text("average cps: [%.2f]", toad::clickrecorder::average_cps);
        ImGui::Text("total clicks: [%i]", toad::clickrecorder::total_clicks);
        ImGui::Text("lines loaded: [%i]", toad::clickrecorder::click_delays.size());

        //ImGui::Text("Starting point: %i", );
        
        ImGui::PushItemWidth(50);
        if (toad::clickrecorder::click_delays.empty())
        {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha / 2);
        }
        ImGui::Text("multiplier");
        ImGui::SameLine();
        if (ImGui::DragFloat("##ClickRecordermultiplier", &toad::clickrecorder::multiplier, 0, 1, 2, "%.2f", 1) && !toad::clickrecorder::click_delays.empty())
            ClickRecorder::get().calc_vars();
        if (toad::clickrecorder::click_delays.empty())
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
        
        if (toad::CheckBoxFill("enabled", &toad::clickrecorder::playback_enabled))
        {
            if (!toad::clickrecorder::playback_enabled)
            {
                ClickRecorder::get().stop_playback_thread();
            }
            else
            {
                ClickRecorder::get().start_playback_thread();
            }
        }

        ImGui::SameLine(); ImGui::TextColored(ImColor(51, 51, 51), "[%s]", &toad::clickrecorder::key_playback);
        if (ImGui::IsItemClicked()) { toad::clickrecorder::key_playback = ".."; binding = true; }
        if (toad::clickrecorder::click_delays.empty()) { ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 0, 0, 1), "There are no clicks loaded"); }
        
        ImGui::PushID("recorderwhenvisib");
        toad::CheckBoxFill("when mouse visible", &toad::clickrecorder::inventory);
        ImGui::PopID();
        toad::CheckBoxFill("randomize start", &toad::clickrecorder::randomize_start_point);
        ImGui::Text("starting point"); ImGui::SameLine();
        ImGui::InputInt("##Playbackstartpoint", &toad::clickrecorder::click_start_point, 2);
        if (toad::clickrecorder::click_start_point < 0 || toad::clickrecorder::click_start_point > toad::clickrecorder::click_delays.size()) toad::clickrecorder::click_start_point = 0;
        if (!toad::clickrecorder::is_start_point_valid()) { toad::clickrecorder::click_start_point++; }
        ImGui::EndChild(); // end of recorderPlayback child
    }

    //misc
    else if (tab == 3)
    {
        ImGui::BeginChild("misc", ImVec2(ImGui::GetWindowSize().x / 2 - 30, 300), true);
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x / 2) - ImGui::CalcTextSize("misc").x + 10);
        ImGui::TextColored(ImColor(122, 122, 122), "misc");

        ImGui::Separator();

        toad::CheckBoxFill("beep on toggle", &toad::misc::beep_on_toggle);
        if (toad::CheckBoxFill("compatibility mode", &toad::misc::compatibility_mode))
            if (toad::misc::compatibility_mode) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
            }
            else 
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
            }

        if (toad::CheckBoxFill("use mouse_event", &toad::misc::use_mouse_event));
       
        ImGui::Text("hide bind"); ImGui::SameLine(); ImGui::TextColored(ImColor(51, 51, 51), "[%s]", &toad::misc::hide_key);
        if (ImGui::IsItemClicked()) { toad::misc::hide_key = ".."; binding = true; }

        ImGui::ColorPicker3("##GuiCol", toad::theme::main_col);

        if (toad::CheckBoxFill("loop hue color", &toad::theme::hue_loop_mode))
            if (!toad::theme::hue_loop_mode)
            {
                toad::theme::main_col[0] = previousColor[0];
                toad::theme::main_col[1] = previousColor[1];
                toad::theme::main_col[2] = previousColor[2];
                toad::theme::main_col[3] = previousColor[3];
            }
            else
            {
                previousColor[0] = toad::theme::main_col[0];
                previousColor[1] = toad::theme::main_col[1];
                previousColor[2] = toad::theme::main_col[2];
                previousColor[3] = toad::theme::main_col[3];
            }
        if (toad::theme::hue_loop_mode)
            ImGui::SliderFloat("##gay amount", &toad::theme::speed, 0.01, 1);

        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::SetCursorPosX(ImGui::GetWindowSize().x - ImVec2(ImGui::GetWindowSize().x / 2 - 30, 200).x - 20);
        ImGui::BeginChild("jitter", ImVec2(ImGui::GetWindowSize().x / 2 - 30, 160), true);
        ImGui::SetCursorPosX(ImGui::GetWindowSize().x / 2 - ImGui::CalcTextSize("jitter").x + 16);
        ImGui::TextColored(ImColor(122, 122, 122), "jitter");

        ImGui::Separator();

        toad::CheckBoxFill("enable", &toad::jitter::enable);
        /*ImGui::Text("intensity x");
        ImGui::SliderInt("##intensity x", &toad::jitter::intensity_X, 1, 10);
        ImGui::Spacing();
        ImGui::Text("intensity y");
        ImGui::SliderInt("##Intensity y", &toad::jitter::intensity_Y, 1, 10);*/
        ImGui::Text("intensity");
        if (ImGui::SliderInt("##intensity", &toad::jitter::intensity_X, 1, 10))
            toad::jitter::intensity_Y = toad::jitter::intensity_X;
        ImGui::Spacing();
        ImGui::Text("chance");
        ImGui::SliderInt("##Chance", &toad::jitter::chance, 1, 100, "%d%%");

        ImGui::EndChild();

        ImGui::SetCursorPosX(ImGui::GetWindowSize().x - ImVec2(ImGui::GetWindowSize().x / 2 - 30, 200).x - 20);
        ImGui::SetCursorPosY(230);
        ImGui::BeginChild("custom window",ImVec2(ImGui::GetWindowSize().x / 2 - 30, 130), true);
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x / 2) - ImGui::CalcTextSize("custom window").x + 10);
        ImGui::TextColored(ImColor(122, 122, 122), "    custom window");

        ImGui::Separator();

        ImGui::Text("clicking window");
        ImGui::Combo("##ClickingWindow", &toad::misc::selected_window_type, toad::misc::window_options, IM_ARRAYSIZE(toad::misc::window_options));

        if (toad::misc::selected_window_type == 2)
        {
            ImGui::InputText("##CustomWindowName", toad::misc::custom_window_title, 50, ImGuiInputTextFlags_ReadOnly);
            ImGui::SameLine();
            if (ImGui::Button("..."))
            {
                toad::misc::proc_list.clear();
                toad::misc::get_window_list();
                showProcessList = !showProcessList;
            }
        }

        if (toad::clicking_window == NULL) ImGui::TextColored(ImVec4(205, 0, 0, 255), "no active window found");
        else ImGui::TextColored(ImVec4(0, 205, 0, 255), "Clicking on PID: %d", toad::misc::pid);

        ImGui::EndChild();
    }

    // settings (os related) 
    else if (tab == 4)
    {
        ImGui::BeginChild("SoundPlayer");
        {
            // sound format settings
            auto& frmt = p_SoundPlayer->GetWaveFormat();
            constexpr const char* preview_channels[] = { "mono", "stereo" };
            static int selected_channel = 0;
            
            center_textX({ 1,1,1,1 }, "sound format");

            ImGui::Separator();

            if (ImGui::InputInt("samples", (int*)frmt.nSamplesPerSec, 100, 96000))
            {
                frmt.nBlockAlign = frmt.nChannels * frmt.wBitsPerSample / 8;
                frmt.nAvgBytesPerSec = frmt.nSamplesPerSec * frmt.nBlockAlign;
            }
            if (ImGui::InputInt("bits per sample", (int*)frmt.wBitsPerSample, 1, 32))
            {
                frmt.nBlockAlign = frmt.nChannels * frmt.wBitsPerSample / 8;
                frmt.nAvgBytesPerSec = frmt.nSamplesPerSec * frmt.nBlockAlign;
            }

            if (ImGui::Combo("channels", &selected_channel, preview_channels, 2))
            {
                selected_channel = std::clamp(selected_channel, 1, 2);
                frmt.nChannels = selected_channel + 1;

                frmt.nBlockAlign = frmt.nChannels * frmt.wBitsPerSample / 8;
                frmt.nAvgBytesPerSec = frmt.nSamplesPerSec * frmt.nBlockAlign;
            }

            ImGui::EndChild();
        }
    }

    ImGui::SetCursorPosY(ImGui::GetWindowSize().y - 20);
    ImGui::TextColored(ImColor(51, 51, 51), "made by vierkant");

    if (showProcessList && toad::misc::selected_window_type == 2)
    {
        ImGui::Begin("Process List", &showProcessList, ImGuiWindowFlags_NoDocking);
        if (ImGui::Button("refresh"))
        {
            toad::misc::proc_list.clear();
            toad::misc::get_window_list();
        }
        for (size_t i = 0; i < toad::misc::proc_list.size(); i++) {
            const bool is_selected = (selected_item == i);
            if (ImGui::Selectable(toad::misc::proc_list[i].name.c_str(), is_selected))
            {
                selected_item = i;
            }
            if (ImGui::IsItemFocused() && ImGui::IsMouseDoubleClicked(0))
            {
                std::strcpy(toad::misc::custom_window_title, toad::misc::proc_list[i].name.c_str());
                toad::misc::pid = toad::misc::proc_list[i].pid;
                toad::clicking_window = toad::misc::proc_list[i].hwnd;
                showProcessList = false;
            }
            if (is_selected)
            {
                if (ImGui::Button("select as active"))
                {
                    std::strcpy(toad::misc::custom_window_title, toad::misc::proc_list[i].name.c_str());
                    toad::misc::pid = toad::misc::proc_list[i].pid;
                    toad::clicking_window = toad::misc::proc_list[i].hwnd;
                    showProcessList = false;
                }
            }
        }
        ImGui::End();
    }

    ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 45);
    ImGui::SetCursorPosY(10);

    ImGui::End();
}