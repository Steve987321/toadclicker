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
            //TODO: fix this
            if (pt.x != dst.x)
            {
                pt.x < dst.x ? move_mouseX(1) : pt.x == dst.x ? pt.x = dst.x : move_mouseX(-1);
                pt.y < dst.y ? move_mouseY(1) : pt.y == dst.y ? pt.y = dst.y : move_mouseY(-1);
                
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
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
        
        p_Jitter.get()->set_jitter_pos();
        
        if (!thread_launched) { std::thread(&Jitter::thread, p_Jitter.get()).detach(); thread_launched = true; }
    }
}