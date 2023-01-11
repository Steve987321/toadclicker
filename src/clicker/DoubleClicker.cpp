#include "pch.h"
#include "DoubleClicker.h"
#include "../toad.h"

void c_doubleClicker::mousedownchecker_thread()
{
	while (thread_flag)
	{
		if (toad::window_is_focused(toad::clicking_window))
		{
			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 && !mouseFlag)
			{
				if (mouseDoubleClickingStage == 2) 
				{ 
					mouseDoubleClickingStage = 3; 
				}
				mouseFlag = true;
			}
			else if (!GetAsyncKeyState(VK_LBUTTON))
			{
				if (mouseDoubleClickingStage == 1) 
				{
					mouseDoubleClickingStage = 2;
				}
				else do_onceFlag = false;
				mouseFlag = false;
			}
		}
	}
}

void c_doubleClicker::thread()
{
	while (thread_flag)
	{
		if (toad::window_is_focused(toad::clicking_window))
		{
			if (mouseFlag && !do_onceFlag)
			{
				std::unique_lock<std::shared_mutex> lock(mutex);
				mouseDoubleClickingStage = 1;
				this->double_click();
			}
		}
	}
}

void c_doubleClicker::double_click()
{
	do_onceFlag = true;
	const int interval = toad::random_int(toad::double_clicker::min_interval, toad::double_clicker::max_interval);
	GetCursorPos(&pt);
	if (toad::random_int(1, 100) < toad::double_clicker::chance)
	{
		for (int i = 0; i < interval; i++, std::this_thread::sleep_for(std::chrono::milliseconds(1)))
		{
			if (mouseDoubleClickingStage == 3) {
				goto s;
			}
		}
		PostMessage(toad::clicking_window, WM_LBUTTONDOWN, MKF_LEFTBUTTONDOWN, LPARAM((pt.x, pt.y)));
	}
s:
	std::this_thread::sleep_for(std::chrono::milliseconds(toad::double_clicker::delay));
	mouseDoubleClickingStage = 0;
}