#include "pch.h"
#include "Jitter.h"
#include "../Toad.h"

bool thread_launched = false;

void Jitter::Reset()
{
    GetCursorPos(&m_origin);
    m_dst = m_origin;
    m_pt = m_origin;
}

void Jitter::MoveMouseX(int i)
{
    POINT p;
    GetCursorPos(&p);
    SetCursorPos(p.x + i, p.y);
    m_pt.x += i;
}

void Jitter::MoveMouseY(int i)
{
    POINT p;
    GetCursorPos(&p);
    SetCursorPos(p.x, p.y + i);
    m_pt.y += i;
}

void Jitter::SetJitterPos()
{
    Reset();
    m_dst.x = m_origin.x + toad::random_int(-toad::jitter::intensity_X, toad::jitter::intensity_X);
    m_dst.y = m_origin.y + toad::random_int(-toad::jitter::intensity_Y, toad::jitter::intensity_Y);
}

void Jitter::Thread()
{
    while (toad::is_running)
    {
        if (toad::jitter::enable && GetAsyncKeyState(VK_LBUTTON) && toad::window_is_focused(toad::clicking_window))
        {
            if (m_pt.x != m_dst.x && m_pt.y != m_dst.y)
            {
                LOG_DEBUGF("moving mouse pt(%lu, %lu) dst(%lu, %lu)", m_pt.x, m_pt.y, m_dst.x, m_dst.y);
                can_setJitter = false;
                if (m_pt.x < m_dst.x) MoveMouseX(1);
                else if (m_pt.x > m_dst.x) MoveMouseX(-1);
                if (m_pt.y < m_dst.y) MoveMouseY(1);
                else if (m_pt.y > m_dst.y) MoveMouseY(-1);
                /*pt.x < dst.x ? move_mouseX(1) : pt.x == dst.x ? pt.x = dst.x : move_mouseX(-1);
                pt.y < dst.y ? move_mouseY(1) : pt.y == dst.y ? pt.y = dst.y : move_mouseY(-1);*/
                LOG_DEBUGF("moved  mouse pt(%lu, %lu) dst(%lu, %lu)", m_pt.x, m_pt.y, m_dst.x, m_dst.y);

                std::this_thread::sleep_for(std::chrono::milliseconds(toad::random_int(5, 15)));
            }
            else
            {
                LOG_DEBUGF("refres mouse pt(%lu, %lu) dst(%lu, %lu)", m_pt.x, m_pt.y, m_dst.x, m_dst.y);
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
        
        if (p_Jitter->can_setJitter) p_Jitter->SetJitterPos();
        
        if (!thread_launched) { std::thread(&Jitter::Thread, p_Jitter.get()).detach(); thread_launched = true; }
    }
}