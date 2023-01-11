#include "pch.h"
#include "Jitter.h"
#include "../toad.h"

bool thread_launched = false;

void Jitter::reset()
{
    GetCursorPos(&origin);
    dst = origin;
    pt = origin;
}

void Jitter::move_mouseX(int i)
{
    POINT p;
    GetCursorPos(&p);
    SetCursorPos(p.x + i, p.y);
    pt.x += i;
}

void Jitter::move_mouseY(int i)
{
    POINT p;
    GetCursorPos(&p);
    SetCursorPos(p.x, p.y + i);
    pt.y += i;
}

void Jitter::set_jitter_pos()
{
    reset();
    dst.x = origin.x + toad::random_int(-toad::jitter::intensity_X, toad::jitter::intensity_X);
    dst.y = origin.y + toad::random_int(-toad::jitter::intensity_Y, toad::jitter::intensity_Y);
}

void Jitter::thread()
{
    while (toad::is_running)
    {
        if (toad::jitter::enable && GetAsyncKeyState(VK_LBUTTON) && toad::window_is_focused(toad::clicking_window))
        {
            if (pt.x != dst.x && pt.y != dst.y)
            {
                log_debugf("moving mouse pt(%lu, %lu) dst(%lu, %lu)", pt.x, pt.y, dst.x, dst.y);
                can_setJitter = false;
                if (pt.x < dst.x) move_mouseX(1);
                else if (pt.x > dst.x) move_mouseX(-1);
                if (pt.y < dst.y) move_mouseY(1);
                else if (pt.y > dst.y) move_mouseY(-1);
                /*pt.x < dst.x ? move_mouseX(1) : pt.x == dst.x ? pt.x = dst.x : move_mouseX(-1);
                pt.y < dst.y ? move_mouseY(1) : pt.y == dst.y ? pt.y = dst.y : move_mouseY(-1);*/
                log_debugf("moved  mouse pt(%lu, %lu) dst(%lu, %lu)", pt.x, pt.y, dst.x, dst.y);

                std::this_thread::sleep_for(std::chrono::milliseconds(toad::random_int(5, 15)));
            }
            else
            {
                log_debugf("refres mouse pt(%lu, %lu) dst(%lu, %lu)", pt.x, pt.y, dst.x, dst.y);
                can_setJitter = true;
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void toad::jitter::do_jitter()
{
    if (toad::random_int(0, 100) < toad::jitter::chance) {
        
        if (p_Jitter->can_setJitter) p_Jitter->set_jitter_pos();
        
        if (!thread_launched) { std::thread(&Jitter::thread, p_Jitter.get()).detach(); thread_launched = true; }
    }
}