#include "pch.h"
#include "MouseHook.h"
#include "toad.h"

#include "app/Application.h"

HHOOK hook = NULL;
MSG msg;

LRESULT _stdcall mousecallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	auto pMouseStruct = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);

    if (nCode == 0)
    {
        switch (wParam)
        {
        case WM_MOUSEWHEEL:
            if ((int)pMouseStruct->mouseData < 0) toad::clicker::curr_slot == 0 ? toad::clicker::curr_slot = 8 : toad::clicker::curr_slot--;
            else if ((int)pMouseStruct->mouseData > 0) toad::clicker::curr_slot == 8 ? toad::clicker::curr_slot = 0 : toad::clicker::curr_slot++;
            break;
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
	        ClickRecorder::get().save_delay();
            break;
        default:
            break;
        }
    }
    return CallNextHookEx(hook, nCode, wParam, lParam);
}

void c_mouseHook::thread()
{
    //init hook
    LOGDEBUG("hooking mouse");
    hook = SetWindowsHookEx(WH_MOUSE_LL, mousecallback, NULL, 0);
    hook ? LOGDEBUG("hook successful") : LOGERROR("failed to set hook");

    while (toad::is_running)
    {
        if (GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }       
    }
}

void c_mouseHook::unhook()
{
#ifndef _DEBUG
    LOGDEBUG("unhooking mouse");
    UnhookWindowsHookEx(hook);
#endif
}
