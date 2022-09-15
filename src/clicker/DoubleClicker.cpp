#include "pch.h"
#include "DoubleClicker.h"
#include "../toad.h"

void c_doubleClicker::thread()
{
	while (thread_flag)
	{
		if (toad::window_is_focused(toad::clicking_window))
		{
			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 && !mouseFlag)
			{
				std::unique_lock<std::shared_mutex> lock(mutex);
				this->double_click();
				mouseFlag = true;
			}
			else if (!GetAsyncKeyState(VK_LBUTTON))
			{
				mouseFlag = false;
			}
		}
	}
}
void c_doubleClicker::double_click()
{
	GetCursorPos(&pt);
	if (toad::random_int(1, 100) < toad::double_clicker::chance)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(toad::random_int(1, 3)));
		PostMessage(toad::clicking_window, WM_LBUTTONDOWN, MKF_LEFTBUTTONDOWN, LPARAM((pt.x, pt.y)));
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(toad::double_clicker::delay));
}