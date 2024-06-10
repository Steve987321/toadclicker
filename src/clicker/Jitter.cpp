#include "pch.h"
#include "Jitter.h"
#include "../Toad.h"

Jitter::~Jitter()
{
    StopThread();
}

void Jitter::DoJitter()
{
	std::unique_lock lock(m_mutex);

	if (toad::random_int(0, 100) < toad::jitter::chance) {

		if (p_Jitter->m_canJitter)
			p_Jitter->SetJitterPos();

		m_cv.notify_one();
	}
}

void Jitter::StartThread()
{
    m_threadFlag = true;
	m_thread = std::thread(&Jitter::Thread, this);
}

void Jitter::StopThread()
{
    {
        std::lock_guard lock(m_mutex);
        m_threadFlag = false;
    }

    m_cv.notify_one();
    if (m_thread.joinable())
        m_thread.join();
}

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
    while (m_threadFlag)
    {
		if (m_pt.x == m_dst.x && m_pt.y == m_dst.y)
		{
			std::unique_lock lock(m_mutex);
			m_cv.wait(lock, [&] { return !m_threadFlag || (toad::jitter::enable && GetAsyncKeyState(VK_LBUTTON) && toad::window_is_focused(toad::clicking_window)); });
        }

        if (!m_threadFlag)
            break;

        //LOG_DEBUGF("moving mouse pt(%lu, %lu) dst(%lu, %lu)", m_pt.x, m_pt.y, m_dst.x, m_dst.y);
        m_canSetJitter = false;
        if (m_pt.x < m_dst.x) MoveMouseX(1);
        else if (m_pt.x > m_dst.x) MoveMouseX(-1);
        if (m_pt.y < m_dst.y) MoveMouseY(1);
        else if (m_pt.y > m_dst.y) MoveMouseY(-1);
        //LOG_DEBUGF("moved mouse pt(%lu, %lu) dst(%lu, %lu)", m_pt.x, m_pt.y, m_dst.x, m_dst.y);

        std::this_thread::sleep_for(std::chrono::milliseconds(toad::random_int(5, 15)));

		m_canSetJitter = true;
    }
}
