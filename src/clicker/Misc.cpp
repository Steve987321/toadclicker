#include "pch.h"
#include "Misc.h"
#include "Toad.h"

namespace toad
{

static BOOL CALLBACK enumWindowCallback(HWND hwnd, LPARAM lparam) {
    const DWORD TITLE_SIZE =     1024;
          DWORD PID =            0;
    std::string title;

    WCHAR windowTitle[TITLE_SIZE];

    GetWindowTextW(hwnd, windowTitle, TITLE_SIZE);

    const int length = ::GetWindowTextLength(hwnd);

    if (IsWindowVisible(hwnd) && length != 0) {

        //convert to std::string
        char* buf = new char[length + 1];
       
        char DefChar = ' ';
        WideCharToMultiByte(CP_ACP, 0, windowTitle, -1, buf, length + 1, &DefChar, NULL);

        title = std::string(buf);
        delete[] buf;

        GetWindowThreadProcessId(hwnd, &PID);
        misc::proc_list.emplace_back(PID, std::move(title), hwnd);

        return TRUE;
    }
    return TRUE;
}

BOOL misc::get_window_list()
{
    return EnumWindows(enumWindowCallback, 0);
}

void misc::window_scanner()
{
    int delay = 5000;
    while (toad::is_running)
    {
        switch (misc::selected_click_window) {
        case 0:
            toad::clicking_window = GetForegroundWindow();
            delay = 3500;
            break;
        case 1:
            toad::clicking_window = FindWindowA("LWJGL", NULL);
            if (toad::clicking_window != NULL) delay = 6000;
            else delay = 3000;
            break;
        case 2:
            if ((toad::clicking_window = FindWindowA(NULL, LPCSTR(custom_window_title))) == NULL)
            {
                toad::clicking_window = NULL;
                strcpy(custom_window_title, "");
            }
            else delay = 5000;
            break;
        }
		if (toad::clicking_window != NULL)
			GetWindowThreadProcessId(toad::clicking_window, &pid);

		std::this_thread::sleep_for(std::chrono::milliseconds(delay));
	}
}

void misc::hide(HWND window) {
    ::ShowWindow(window, SW_MINIMIZE);
    ::ShowWindow(window, SW_HIDE);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

void misc::show(HWND window) {
    ::ShowWindow(window, SW_SHOWDEFAULT);
    ::ShowWindow(window, SW_SHOW);
    ::ShowWindow(window, SW_HIDE);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

}
