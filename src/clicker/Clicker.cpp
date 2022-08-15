#include "pch.h"
#include "Clicker.h"
#include "../toad.h"

void c_clicker::reset_vars()
{
    this->fonce = false;
    this->type2counter = 0;
    this->type2counter2 = 0;
    this->type2drop2 = false;
    this->type2boost = false;
    this->type2boost2 = false;
    this->type2counterboost = 0;
    this->type2counterboost2 = 0;
    this->first_click = false;
    this->inconsistensy = false;
    this->inconsistensy2 = false;
    this->inconsistensycounter = 0;
    this->inconsistensycounter2 = 0;
}


// handles the randomazation and sending clicks

void c_clicker::send_down(mouse_type mb, POINT& pt, float& sometingdelay, float delayclick2)
{
    GetCursorPos(&pt);
    this->first_click = true;

    if (this->inconsistensy) {
        sometingdelay += toad::random_int(30.f, 100.f);
        this->inconsistensy = false;
    }
    else
        sometingdelay = toad::random_float(this->min2, this->max2);

    if (toad::clicker::blatant_mode)
        std::this_thread::sleep_for(std::chrono::milliseconds((int)delayclick2));
    else
        std::this_thread::sleep_for(std::chrono::milliseconds((int)sometingdelay));

    if (toad::jitter::enable) toad::jitter::do_jitter();

   if (mb == mouse_type::LEFT && toad::clicker::selectedEnableOption == 1) mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
   else if (mb == mouse_type::RIGHT && toad::clicker::r::right_selectedEnableOption == 1) mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
   else mb == mouse_type::LEFT ? PostMessage(toad::clicking_window, WM_LBUTTONDOWN, MKF_LEFTBUTTONDOWN, LPARAM((pt.x, pt.y))) :
        PostMessage(toad::clicking_window, WM_RBUTTONDOWN, MKF_RIGHTBUTTONDOWN, LPARAM((pt.x, pt.y)));
}
void c_clicker::send_up(mouse_type mb,const POINT& pt,float& delay, float delayclick2)
{
    if (this->inconsistensy2) {
        delay += toad::random_int(30.f, 100.f);
        this->inconsistensy2 = false;
    }
    else
        delay = toad::random_float(this->min2, this->max2);

    if (toad::clicker::blatant_mode)
        std::this_thread::sleep_for(std::chrono::milliseconds((int)delayclick2));
    else
        std::this_thread::sleep_for(std::chrono::milliseconds((int)delay));

    if (mb == mouse_type::LEFT && toad::clicker::selectedEnableOption == 1) mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
    else if (mb == mouse_type::RIGHT && toad::clicker::r::right_selectedEnableOption == 1) mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
    else mb == mouse_type::LEFT ? PostMessage(toad::clicking_window, WM_LBUTTONUP, 0, LPARAM((pt.x, pt.y))) :
        PostMessage(toad::clicking_window, WM_RBUTTONUP, 0, LPARAM((pt.x, pt.y)));

    if (toad::jitter::enable) toad::jitter::do_jitter();

    this->inconsistensycounter2++;
    this->inconsistensycounter++;
    this->type2counter++;
    this->type2counterboost++;

    if (this->inconsistensycounter2 > 35) {
        if (toad::random_int(0, 100) < 6) { this->inconsistensy2 = true;  this->inconsistensycounter2 = 0; }
    }
    if (this->inconsistensycounter > 25) {
        if (toad::random_int(0, 100) < 6) { this->inconsistensy = true;  this->inconsistensycounter = 0; }
    }

    if (this->type2counterboost > toad::random_float(this->mincanboostchance, 25)) {
        if (!this->type2drop2 && !this->type2counter2 > 0) {
            this->type2boost = true;
            this->mincanboostchance = 15;
        }
    }
    if (this->type2boost) {
        if (!toad::clicker::higher_cps) { this->mincandropchance -= 0.5f; }
        else if (toad::clicker::higher_cps) { this->mincandropchance -= 0.1f; }

        this->type2counterboost = 0;
        this->min2 -= 1.4f;
        this->max2 += 0.6f;
        this->type2counterboost2++;
    }
    if (this->type2counterboost2 > 5) {
        this->type2counterboost = 0;
        this->type2boost = false;
        this->type2counterboost2++;
        if (type2counterboost2 > 45) {
            this->min2 += 1.4f;
            this->max2 -= 0.6f;
            if (this->type2counterboost2 > 50) {
                this->type2counterboost = 0;
                this->type2counterboost2 = 0;
                this->type2counter = 0;
                this->type2counter2 = 0;
            }
        }
    }
    //drop
    if (this->type2counter > toad::random_float(this->mincandropchance, 20)) {
        if (!this->type2boost && !this->type2counterboost2 > 0) {
            this->type2drop2 = true;
            this->mincandropchance = 15;
        }
    }
    if (type2drop2) {
        if (toad::clicker::higher_cps) { this->mincanboostchance -= 0.5f; }
        else if (!toad::clicker::higher_cps) { this->mincanboostchance -= 0.1f; }
        this->type2counter = 0;
        this->max2 += 1.6f;
        this->type2counter2++;
    }
    if (this->type2counter2 > 5) {
        this->type2counter = 0;
        this->type2drop2 = false;
        this->type2counter2++;
        if (this->type2counter2 > 45) {
            this->max2 -= 1.6f;
            if (this->type2counter2 > 50) {
                this->type2counter = 0;
                this->type2counter2 = 0;
                this->type2counterboost = 0;
                this->type2counterboost2 = 0;
            }
        }
    }
}

// main left clicker thread
void c_clicker::thread(){

    POINT pt;

    while (toad::is_running) {
        //cpu
        if (!toad::clicker::enabled) { std::this_thread::sleep_for(std::chrono::milliseconds(100)); }

        else if (toad::clicker::enabled) {
            
            if (toad::clicker::slot_whitelist && !toad::clicker::whitelisted_slots[toad::clicker::curr_slot]) {std::this_thread::sleep_for(std::chrono::milliseconds(50)); continue; } 
            if (toad::clicker::rmb_lock && !GetAsyncKeyState(VK_RBUTTON)) this->can_stop = true;
            if (!this->can_stop) { std::this_thread::sleep_for(std::chrono::milliseconds(50)); continue; }
            if (!toad::clicker::inventory && toad::clicker::cursor_visible) { std::this_thread::sleep_for(std::chrono::milliseconds(50)); continue; }
            
            if (GetForegroundWindow() == toad::clicking_window)
            {
                if (!GetAsyncKeyState(VK_LBUTTON))
                {
                    //reset vars
                    this->reset_vars();
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }

                //windows 2004/2h20
                float delaymin2 = ((1000 / toad::clicker::maxcps) / 2) - 1.f;
                float delaymax2 = ((1000 / toad::clicker::mincps) / 2) - 1.f;

                float delayclick2 = toad::random_float(delaymin2 - 0.6f, delaymax2 + 1.f);
                float sometingdelay = toad::random_float(this->min2, this->max2);

                if (!this->fonce)
                {
                    this->min2 = delaymin2;
                    this->max2 = delaymax2;
                    this->fonce = true;
                }
                switch (toad::clicker::selectedEnableOption)
                {
                case 0:
                {
                    if (GetAsyncKeyState(VK_LBUTTON))
                    {
                        //delay on first click
                        if (!this->first_click)
                            std::this_thread::sleep_for(std::chrono::milliseconds(int(delayclick2) + 40));
                    }

                    if (GetAsyncKeyState(VK_LBUTTON))
                    {
                        send_down(mouse_type::LEFT, pt, sometingdelay, delayclick2);
                        if (toad::clicker::rmb_lock && GetAsyncKeyState(VK_RBUTTON) && can_stop) { this->can_stop = false; continue; }
                        send_up(mouse_type::LEFT, pt, sometingdelay, delayclick2);
                    }
                }
                break;

                case 1:
                {
                    if (GetAsyncKeyState(toad::clicker::keycode))
                    {
                        send_down(mouse_type::LEFT, pt, sometingdelay, delayclick2);
                        send_up(mouse_type::LEFT, pt, sometingdelay, delayclick2);
                    }
                    else
                    {
                        this->reset_vars();
                    }
                }
                break;

                default:
                    send_down(mouse_type::LEFT, pt, sometingdelay, delayclick2);
                    send_up(mouse_type::LEFT, pt, sometingdelay, delayclick2);
                    break;
                }
            }
            //cpu
            else std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

//main right clicker thread
void c_right_clicker::thread()
{
    while (toad::is_running) {
        //cpu
        if (!toad::clicker::r::right_enabled) { std::this_thread::sleep_for(std::chrono::milliseconds(50)); }

        if (toad::clicker::r::right_enabled) {
            if (!toad::clicker::r::right_inventory && toad::clicker::cursor_visible) { std::this_thread::sleep_for(std::chrono::milliseconds(50)); continue; }
            if (toad::clicker::r::right_only_inventory && !toad::clicker::cursor_visible) { std::this_thread::sleep_for(std::chrono::milliseconds(50)); continue; }

            if (GetForegroundWindow() == toad::clicking_window)
            {
                POINT pt;

                if (!GetAsyncKeyState(VK_RBUTTON))
                {
                    //reset vars
                    this->reset_vars();
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }

                //windows 2004/2h20
                float delaymin2 = ((1000 / toad::clicker::r::right_maxcps) / 2) - 1.f;
                float delaymax2 = ((1000 / toad::clicker::r::right_mincps) / 2) - 1.f;

                float delayclick2 = toad::random_float(delaymin2 - 0.6f, delaymax2 + 1.f);
                float sometingdelay = toad::random_float(this->min2, this->max2);
                if (!fonce)
                {
                    this->min2 = delaymin2;
                    this->max2 = delaymax2;
                    this->fonce = true;
                }

                if (toad::clicker::r::right_selectedEnableOption == 0)
                {
                    if (GetAsyncKeyState(VK_RBUTTON))
                    {
                        //delay on first click
                        if (!this->first_click)
                            std::this_thread::sleep_for(std::chrono::milliseconds(int(delayclick2) + 40));
                    }

                    if (GetAsyncKeyState(VK_RBUTTON))
                    {
                        send_down(mouse_type::RIGHT, pt, sometingdelay, delayclick2);
                        send_up(mouse_type::RIGHT, pt, sometingdelay, delayclick2);
                    }
                }
                else if (toad::clicker::r::right_selectedEnableOption == 1)
                {
                    if (GetAsyncKeyState(toad::clicker::r::right_keycode))
                    {
                        send_down(mouse_type::RIGHT, pt, sometingdelay, delayclick2);
                        send_up(mouse_type::RIGHT, pt, sometingdelay, delayclick2);
                    }
                    else
                    {
                        this->reset_vars();
                    }
                }
                else
                {
                    send_down(mouse_type::RIGHT, pt, sometingdelay, delayclick2);
                    send_up(mouse_type::RIGHT, pt, sometingdelay, delayclick2);
                }
            }
            else
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }   
}
