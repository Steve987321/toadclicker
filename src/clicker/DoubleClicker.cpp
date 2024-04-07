#include "pch.h"
#include "DoubleClicker.h"
#include "../Toad.h"

void DoubleClicker::StartThread()
{
	m_threadFlag = true;
	m_thread = std::thread{ &DoubleClicker::Thread, this };
	m_thread2 = std::thread{ &DoubleClicker::MouseDownCheckerThread, this };
}

void DoubleClicker::StopThread()
{
	m_threadFlag = false;
	if (m_thread.joinable()) m_thread.join();
	if (m_thread2.joinable()) m_thread2.join();
}

bool DoubleClicker::IsThreadAlive() const
{
	return m_threadFlag;
}

void DoubleClicker::Thread()
{
	while (m_threadFlag)
	{
		if (toad::window_is_focused(toad::clicking_window))
		{
			if (m_mouseFlag && !m_doOnceFlag)
			{
				std::unique_lock<std::shared_mutex> lock(m_mutex);
				m_mouseDoubleClickingStage = 1;
				this->DoubleClick();
			}
		}
	}
}

void DoubleClicker::DoubleClick()
{
	m_doOnceFlag = true;
	const int interval = toad::random_int(toad::double_clicker::min_interval, toad::double_clicker::max_interval);
	GetCursorPos(&m_pt);
	if (toad::random_int(1, 100) < toad::double_clicker::chance)
	{
		for (int i = 0; i < interval; i++, std::this_thread::sleep_for(std::chrono::milliseconds(1)))
		{
			if (m_mouseDoubleClickingStage == 3) {
				goto s;
			}
		}
		PostMessage(toad::clicking_window, WM_LBUTTONDOWN, MKF_LEFTBUTTONDOWN, LPARAM((m_pt.x, m_pt.y)));
	}
s:
	std::this_thread::sleep_for(std::chrono::milliseconds(toad::double_clicker::delay));
	m_mouseDoubleClickingStage = 0;
}

void DoubleClicker::MouseDownCheckerThread()
{
	while (m_threadFlag)
	{
		if (toad::window_is_focused(toad::clicking_window))
		{
			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 && !m_mouseFlag)
			{
				if (m_mouseDoubleClickingStage == 2)
				{
					m_mouseDoubleClickingStage = 3;
				}
				m_mouseFlag = true;
			}
			else if (!GetAsyncKeyState(VK_LBUTTON))
			{
				if (m_mouseDoubleClickingStage == 1)
				{
					m_mouseDoubleClickingStage = 2;
				}
				else m_doOnceFlag = false;
				m_mouseFlag = false;
			}
		}
	}
}
