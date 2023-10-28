#include "pch.h"
#include "Clicker.h"
#include "../toad.h"

void c_clicker::reset_vars()
{
    bonce = false;
    counter = 0;
    counter2 = 0;
    bdrop = false;
    boost = false;
    boost2 = false;
    boost_counter = 0;
    boost_counter2 = 0;
    first_click = false;
    inconsistensy = false;
    inconsistensy2 = false;
    inconsistensycounter = 0;
    inconsistensycounter2 = 0;
}

// handles the randomazation and sending clicks
void c_clicker::send_down(mouse_type mb, POINT& pt, float& delay, float delayclick2)
{
    first_click = true;

    if (inconsistensy) {
        delay += toad::random_float(30.f, 100.f);
        inconsistensy = false;
    }
    else
        delay = toad::random_float(min, max);

    if (toad::clicksounds::enabled) toad::clicksounds::play = true;

    if (toad::clicker::blatant_mode)
        std::this_thread::sleep_for(std::chrono::milliseconds((int)delayclick2));
    else
        std::this_thread::sleep_for(std::chrono::milliseconds((int)delay));

    if (toad::jitter::enable && !toad::clicker::cursor_visible) toad::jitter::do_jitter();             
    
    GetCursorPos(&pt);
    if (toad::misc::use_mouse_event)
    {
        mb == mouse_type::LEFT ? mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0)
            : mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
    }
    else mb == mouse_type::LEFT ? PostMessage(toad::clicking_window, WM_LBUTTONDOWN, MKF_LEFTBUTTONDOWN, LPARAM((pt.x, pt.y))) :
        PostMessage(toad::clicking_window, WM_RBUTTONDOWN, MKF_RIGHTBUTTONDOWN, LPARAM((pt.x, pt.y)));
}
   
void c_clicker::send_up(mouse_type mb, POINT& pt,float& delay, float delayclick2)
{
    if (inconsistensy2) {
        delay += toad::random_int(30.f, 100.f);
        inconsistensy2 = false;
    }
    else
        delay = toad::random_float(min, max);

    if (toad::clicker::blatant_mode)
        std::this_thread::sleep_for(std::chrono::milliseconds((int)delayclick2));
    else
        std::this_thread::sleep_for(std::chrono::milliseconds((int)delay));

    GetCursorPos(&pt);
    if (toad::misc::use_mouse_event)
    {
        mb == mouse_type::LEFT ? mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0)
            : mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
    }
    else mb == mouse_type::LEFT ? PostMessage(toad::clicking_window, WM_LBUTTONUP, 0, LPARAM((pt.x, pt.y))) :
        PostMessage(toad::clicking_window, WM_RBUTTONUP, 0, LPARAM((pt.x, pt.y)));

    inconsistensycounter2++;
    inconsistensycounter++;
    counter++;
    boost_counter++;

    if (inconsistensycounter2 > 35) {
        if (toad::random_int(0, 100) < 6) { inconsistensy2 = true;  inconsistensycounter2 = 0; }
    }
    if (inconsistensycounter > 25) {
        if (toad::random_int(0, 100) < 6) { inconsistensy = true;  inconsistensycounter = 0; }
    }

    if (boost_counter > toad::random_float(mincanboostchance, 25)) {
        if (!bdrop && !(counter2 > 0)) {
            boost = true;
            mincanboostchance = 15;
        }
    }
    if (boost) {
        if (toad::clicker::higher_cps) { mincandropchance -= 0.5f; }
        else if (!toad::clicker::higher_cps) { mincandropchance -= 0.1f; }

        boost_counter = 0;
        min -= 1.4f;
        max += 0.6f;
        boost_counter2++;
    }
    if (boost_counter2 > 5) {
        boost_counter = 0;
        boost = false;
        boost_counter2++;
        if (boost_counter2 > 45) {
            min += 1.4f;
            max -= 0.6f;
            if (boost_counter2 > 50) {
                boost_counter = 0;
                boost_counter2 = 0;
                counter = 0;
                counter2 = 0;
            }
        }
    }
    //drop
    if (counter > toad::random_float(mincandropchance, 20)) {
        if (!boost && !(boost_counter2 > 0)) {
            bdrop = true;
            mincandropchance = 15;
        }
    }
    if (bdrop) {
        if (!toad::clicker::higher_cps) { mincanboostchance -= 0.5f; }
        else if (toad::clicker::higher_cps) { mincanboostchance -= 0.1f; }

        counter = 0;
        max += 1.6f;
        counter2++;
    }
    if (counter2 > 5) {
        counter = 0;
        bdrop = false;
        counter2++;
        if (counter2 > 45) {
            max -= 1.6f;
            if (counter2 > 50) {
                counter = 0;
                counter2 = 0;
                boost_counter = 0;
                boost_counter2 = 0;
            }
        }
    }
}

// main clicker thread
void c_clicker::thread(){

    bool playsoundFlag = false;
    POINT pt;
    constexpr auto mb = mouse_type::LEFT;

    float delaymin, delaymax;
    float delayclick, blatantdelay;
    
    while (m_threadFlag) {
        if (toad::clicker::rmb_lock && !GetAsyncKeyState(VK_RBUTTON)) can_stop = true;
        if (!can_stop) { std::this_thread::sleep_for(std::chrono::milliseconds(50)); continue; }
        if (!toad::clicker::inventory && toad::clicker::cursor_visible) { std::this_thread::sleep_for(std::chrono::milliseconds(50)); continue; }

        if (toad::window_is_focused(toad::clicking_window))
        {
            if (toad::clicker::slot_whitelist && !toad::clicker::cursor_visible)
            {
                for (int i = 0; i < toad::hotbar_vkcodes.size(); i++)
                {
                    if (GetAsyncKeyState(toad::hotbar_vkcodes[i]) & 1)
                    {
                        toad::clicker::curr_slot = i;
                    }
                }
                if (!toad::clicker::whitelisted_slots[toad::clicker::curr_slot])
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    continue;
                }
            }

            if (!GetAsyncKeyState(VK_LBUTTON) && toad::clicker::selected_enable_option == 0)
            {
                if (toad::clicksounds::enabled && !playsoundFlag)
                {
                    toad::clicksounds::play = true;
                    playsoundFlag = true;
                }

                reset_vars();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
                
            if (toad::clicker::one_slider)
            {
                delaymin = toad::misc::compatibility_mode ? ((1000 / toad::clicker::cps + 2) / 2) - 1.f : ((1000 / toad::clicker::cps + 2) / 2);
                delaymax = toad::misc::compatibility_mode ? ((1000 / toad::clicker::cps - 2) / 2) - 1.f : ((1000 / toad::clicker::cps - 2) / 2);
            }
            else
            {
                delaymin = toad::misc::compatibility_mode ? ((1000 / toad::clicker::max_cps) / 2) - 1.f : ((1000 / toad::clicker::max_cps) / 2);
                delaymax = toad::misc::compatibility_mode ? ((1000 / toad::clicker::min_cps) / 2) - 1.f : ((1000 / toad::clicker::min_cps) / 2);
            }
              
            delayclick = toad::random_float(delaymin - 0.6f, delaymax + 1.f);
            blatantdelay = toad::random_float(delaymin, delaymax);

            if (!bonce)
            {
                min = delaymin;
                max = delaymax;
                bonce = true;
            }
            switch (toad::clicker::selected_enable_option)
            {
            case 0:
            {
                if (GetAsyncKeyState(VK_LBUTTON))
                {
                    playsoundFlag = false;
                    //delay on first click
                    if (!first_click)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(int(delayclick) + 10));
                        first_click = true;
                    }
                }

                if (GetAsyncKeyState(VK_LBUTTON))
                {
                    send_down(mb, pt, blatantdelay, delayclick);
                    if (toad::clicker::rmb_lock && GetAsyncKeyState(VK_RBUTTON) && can_stop) { can_stop = false; continue; }
                    send_up(mb, pt, blatantdelay, delayclick);
                }
            }
            break;

            case 1:
            {
                if (GetAsyncKeyState(toad::clicker::keycode))
                {
                    send_down(mb, pt, blatantdelay, delayclick);
                    send_up(mb, pt, blatantdelay, delayclick);
                }
                else
                {
                    reset_vars();
                }
            }
            break;

            default:
            case 2:
                send_down(mb, pt, blatantdelay, delayclick);
                send_up(mb, pt, blatantdelay, delayclick);
                break;
            }
        }
        //cpu
        else std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void c_right_clicker::thread_right()
{
    bool playsoundFlag = false;
    POINT pt;
    constexpr auto mb = mouse_type::RIGHT;

    float delaymin, delaymax;
    float delayclick, blatantdelay;

    while (m_threadFlag) {
        //cpu
       // if (!toad::clicker::r::right_enabled) { std::this_thread::sleep_for(std::chrono::milliseconds(100)); }
        // unnesesarry checks (thread is active when enabled is true)
       // else {
             
            if (!can_stop) { std::this_thread::sleep_for(std::chrono::milliseconds(50)); continue; }
            if (toad::clicker::r::only_inventory && !toad::clicker::cursor_visible) { std::this_thread::sleep_for(std::chrono::milliseconds(50)); continue; }
            if (!toad::clicker::r::inventory && toad::clicker::cursor_visible) { std::this_thread::sleep_for(std::chrono::milliseconds(50)); continue; }

            if (GetForegroundWindow() == toad::clicking_window)
            {
                if (!GetAsyncKeyState(VK_RBUTTON))
                {
                    if (toad::clicksounds::enabled && !playsoundFlag)
                    {
                        toad::clicksounds::play = true;
                        playsoundFlag = true;
                    }

                    reset_vars();
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }

                delaymin = toad::misc::compatibility_mode ? ((1000 / toad::clicker::r::max_cps) / 2) - 1.f : ((1000 / toad::clicker::r::max_cps) / 2);
                delaymax = toad::misc::compatibility_mode ? ((1000 / toad::clicker::r::min_cps) / 2) - 1.f : ((1000 / toad::clicker::r::min_cps) / 2);

                delayclick = toad::random_float(delaymin - 0.6f, delaymax + 1.f);
                blatantdelay = toad::random_float(delaymin, delaymax);

                if (!bonce)
                {
                    min = delaymin;
                    max = delaymax;
                    bonce = true;
                }
                switch (toad::clicker::r::selected_enable_option)
                {
                case 0:
                {
                    if (GetAsyncKeyState(VK_RBUTTON))
                    {
                        playsoundFlag = false;
                        //delay on first click
                        if (!first_click)
                        {
                            std::this_thread::sleep_for(std::chrono::milliseconds(int(delayclick) + 10));
                            first_click = true;
                        }
                    }

                    if (GetAsyncKeyState(VK_RBUTTON))
                    {
                        send_down(mb, pt, blatantdelay, delayclick);
                        send_up(mb, pt, blatantdelay, delayclick);
                    }
                }
                break;

                case 1:
                {
                    if (GetAsyncKeyState(toad::clicker::r::keycode))
                    {
                        send_down(mb, pt, blatantdelay, delayclick);
                        send_up(mb, pt, blatantdelay, delayclick);
                    }
                    else
                    {
                        reset_vars();
                    }
                }
                break;

                default:
                    send_down(mb, pt, blatantdelay, delayclick);
                    send_up(mb, pt, blatantdelay, delayclick);
                    break;
                }
            }
            //cpu
            else std::this_thread::sleep_for(std::chrono::milliseconds(500));
       // }
    }
}
