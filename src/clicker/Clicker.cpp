#include "pch.h"
#include "Clicker.h"
#include "../toad.h"

void c_clicker::reset_vars()
{
    this->bonce = false;
    this->counter = 0;
    this->counter2 = 0;
    this->bdrop = false;
    this->boost = false;
    this->boost2 = false;
    this->boost_counter = 0;
    this->boost_counter2 = 0;
    this->first_click = false;
    this->inconsistensy = false;
    this->inconsistensy2 = false;
    this->inconsistensycounter = 0;
    this->inconsistensycounter2 = 0;
}

// handles the randomazation and sending clicks
void c_clicker::send_down(mouse_type mb, POINT& pt, float& delay, float delayclick2)
{
    this->first_click = true;

    if (this->inconsistensy) {
        delay += toad::random_float(30.f, 100.f);
        this->inconsistensy = false;
    }
    else
        delay = toad::random_float(this->min, this->max);

    if (toad::clicksounds::enabled) toad::clicksounds::play = true;

    if (toad::clicker::blatant_mode)
        std::this_thread::sleep_for(std::chrono::milliseconds((int)delayclick2));
    else
        std::this_thread::sleep_for(std::chrono::milliseconds((int)delay));

    if (toad::jitter::enable && !toad::clicker::cursor_visible) toad::jitter::do_jitter();             
    
    GetCursorPos(&pt);
    if (toad::misc::use_mouseEvent)
    {
        mb == mouse_type::LEFT ? mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0)
            : mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
    }
    else mb == mouse_type::LEFT ? PostMessage(toad::clicking_window, WM_LBUTTONDOWN, MKF_LEFTBUTTONDOWN, LPARAM((pt.x, pt.y))) :
        PostMessage(toad::clicking_window, WM_RBUTTONDOWN, MKF_RIGHTBUTTONDOWN, LPARAM((pt.x, pt.y)));
}
   
void c_clicker::send_up(mouse_type mb, POINT& pt,float& delay, float delayclick2)
{
    if (this->inconsistensy2) {
        delay += toad::random_int(30.f, 100.f);
        this->inconsistensy2 = false;
    }
    else
        delay = toad::random_float(this->min, this->max);

    if (toad::clicker::blatant_mode)
        std::this_thread::sleep_for(std::chrono::milliseconds((int)delayclick2));
    else
        std::this_thread::sleep_for(std::chrono::milliseconds((int)delay));

    GetCursorPos(&pt);
    if (toad::misc::use_mouseEvent)
    {
        mb == mouse_type::LEFT ? mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0)
            : mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
    }
    else mb == mouse_type::LEFT ? PostMessage(toad::clicking_window, WM_LBUTTONUP, 0, LPARAM((pt.x, pt.y))) :
        PostMessage(toad::clicking_window, WM_RBUTTONUP, 0, LPARAM((pt.x, pt.y)));

    this->inconsistensycounter2++;
    this->inconsistensycounter++;
    this->counter++;
    this->boost_counter++;

    if (this->inconsistensycounter2 > 35) {
        if (toad::random_int(0, 100) < 6) { this->inconsistensy2 = true;  this->inconsistensycounter2 = 0; }
    }
    if (this->inconsistensycounter > 25) {
        if (toad::random_int(0, 100) < 6) { this->inconsistensy = true;  this->inconsistensycounter = 0; }
    }

    if (this->boost_counter > toad::random_float(this->mincanboostchance, 25)) {
        if (!this->bdrop && !this->counter2 > 0) {
            this->boost = true;
            this->mincanboostchance = 15;
        }
    }
    if (this->boost) {
        if (toad::clicker::higher_cps) { this->mincandropchance -= 0.5f; }
        else if (!toad::clicker::higher_cps) { this->mincandropchance -= 0.1f; }

        this->boost_counter = 0;
        this->min -= 1.4f;
        this->max += 0.6f;
        this->boost_counter2++;
    }
    if (this->boost_counter2 > 5) {
        this->boost_counter = 0;
        this->boost = false;
        this->boost_counter2++;
        if (boost_counter2 > 45) {
            this->min += 1.4f;
            this->max -= 0.6f;
            if (this->boost_counter2 > 50) {
                this->boost_counter = 0;
                this->boost_counter2 = 0;
                this->counter = 0;
                this->counter2 = 0;
            }
        }
    }
    //drop
    if (this->counter > toad::random_float(this->mincandropchance, 20)) {
        if (!this->boost && !this->boost_counter2 > 0) {
            this->bdrop = true;
            this->mincandropchance = 15;
        }
    }
    if (bdrop) {
        if (!toad::clicker::higher_cps) { this->mincanboostchance -= 0.5f; }
        else if (toad::clicker::higher_cps) { this->mincanboostchance -= 0.1f; }

        this->counter = 0;
        this->max += 1.6f;
        this->counter2++;
    }
    if (this->counter2 > 5) {
        this->counter = 0;
        this->bdrop = false;
        this->counter2++;
        if (this->counter2 > 45) {
            this->max -= 1.6f;
            if (this->counter2 > 50) {
                this->counter = 0;
                this->counter2 = 0;
                this->boost_counter = 0;
                this->boost_counter2 = 0;
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
    

    SetThreadPriority(mthread.native_handle(), THREAD_PRIORITY_ABOVE_NORMAL);

    while (m_threadFlag) {
        //cpu
        //if (!toad::clicker::enabled) { std::this_thread::sleep_for(std::chrono::milliseconds(100)); }
        // unnesesarry checks (thread is active when enabled is true)
        //else if (toad::clicker::enabled) {

            if (toad::clicker::slot_whitelist && !toad::clicker::whitelisted_slots[toad::clicker::curr_slot]) {std::this_thread::sleep_for(std::chrono::milliseconds(50)); continue; } 
            if (toad::clicker::rmb_lock && !GetAsyncKeyState(VK_RBUTTON)) this->can_stop = true;
            if (!this->can_stop) { std::this_thread::sleep_for(std::chrono::milliseconds(50)); continue; }
            if (!toad::clicker::inventory && toad::clicker::cursor_visible) { std::this_thread::sleep_for(std::chrono::milliseconds(50)); continue; }

            if (GetForegroundWindow() == toad::clicking_window)
            {

                if (!GetAsyncKeyState(VK_LBUTTON) && toad::clicker::selectedEnableOption == 0)
                {
                    if (toad::clicksounds::enabled && !playsoundFlag)
                    {
                        toad::clicksounds::play = true;
                        playsoundFlag = true;
                    }

                    this->reset_vars();
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
                
                if (toad::clicker::one_slider)
                {
                    delaymin = toad::misc::compatibility_mode ? ((1000 / toad::clicker::cps + 2) / 2) - 1.f : ((1000 / toad::clicker::cps + 2) / 2);
                    delaymax = toad::misc::compatibility_mode ? ((1000 / toad::clicker::cps - 2) / 2) - 1.f : ((1000 / toad::clicker::cps - 2) / 2);
                }
                else
                {
                    delaymin = toad::misc::compatibility_mode ? ((1000 / toad::clicker::maxcps) / 2) - 1.f : ((1000 / toad::clicker::maxcps) / 2);
                    delaymax = toad::misc::compatibility_mode ? ((1000 / toad::clicker::mincps) / 2) - 1.f : ((1000 / toad::clicker::mincps) / 2);
                }
              
                delayclick = toad::random_float(delaymin - 0.6f, delaymax + 1.f);
                blatantdelay = toad::random_float(delaymin, delaymax);

                if (!this->bonce)
                {
                    this->min = delaymin;
                    this->max = delaymax;
                    this->bonce = true;
                }
                switch (toad::clicker::selectedEnableOption)
                {
                case 0:
                {
                    if (GetAsyncKeyState(VK_LBUTTON))
                    {
                        playsoundFlag = false;
                        //delay on first click
                        if (!this->first_click)
                        {
                            std::this_thread::sleep_for(std::chrono::milliseconds(int(delayclick) + 10));
                            first_click = true;
                        }
                    }

                    if (GetAsyncKeyState(VK_LBUTTON))
                    {
                        send_down(mb, pt, blatantdelay, delayclick);
                        if (toad::clicker::rmb_lock && GetAsyncKeyState(VK_RBUTTON) && can_stop) { this->can_stop = false; continue; }
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
                        this->reset_vars();
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
        //}
    }
}

void c_right_clicker::thread_right()
{
    bool playsoundFlag = false;
    POINT pt;
    constexpr auto mb = mouse_type::RIGHT;

    float delaymin, delaymax;
    float delayclick, blatantdelay;

    SetThreadPriority(mthread.native_handle(), THREAD_PRIORITY_ABOVE_NORMAL);

    while (m_threadFlag) {
        //cpu
       // if (!toad::clicker::r::right_enabled) { std::this_thread::sleep_for(std::chrono::milliseconds(100)); }
        // unnesesarry checks (thread is active when enabled is true)
       // else {
             
            if (!this->can_stop) { std::this_thread::sleep_for(std::chrono::milliseconds(50)); continue; }
            if (toad::clicker::r::right_only_inventory && !toad::clicker::cursor_visible) { std::this_thread::sleep_for(std::chrono::milliseconds(50)); continue; }
            if (!toad::clicker::r::right_inventory && toad::clicker::cursor_visible) { std::this_thread::sleep_for(std::chrono::milliseconds(50)); continue; }

            if (GetForegroundWindow() == toad::clicking_window)
            {
                if (!GetAsyncKeyState(VK_RBUTTON))
                {
                    if (toad::clicksounds::enabled && !playsoundFlag)
                    {
                        toad::clicksounds::play = true;
                        playsoundFlag = true;
                    }

                    this->reset_vars();
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }

                delaymin = toad::misc::compatibility_mode ? ((1000 / toad::clicker::r::right_maxcps) / 2) - 1.f : ((1000 / toad::clicker::r::right_maxcps) / 2);
                delaymax = toad::misc::compatibility_mode ? ((1000 / toad::clicker::r::right_mincps) / 2) - 1.f : ((1000 / toad::clicker::r::right_mincps) / 2);

                delayclick = toad::random_float(delaymin - 0.6f, delaymax + 1.f);
                blatantdelay = toad::random_float(delaymin, delaymax);

                if (!this->bonce)
                {
                    this->min = delaymin;
                    this->max = delaymax;
                    this->bonce = true;
                }
                switch (toad::clicker::r::right_selectedEnableOption)
                {
                case 0:
                {
                    if (GetAsyncKeyState(VK_RBUTTON))
                    {
                        playsoundFlag = false;
                        //delay on first click
                        if (!this->first_click)
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
                    if (GetAsyncKeyState(toad::clicker::r::right_keycode))
                    {
                        send_down(mb, pt, blatantdelay, delayclick);
                        send_up(mb, pt, blatantdelay, delayclick);
                    }
                    else
                    {
                        this->reset_vars();
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
