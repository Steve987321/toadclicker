#include "pch.h"
#include "MouseHook.h"
#include "../Toad.h"

HHOOK hook = NULL;
MSG msg;

static LRESULT _stdcall mousecallback(int nCode, WPARAM wParam, LPARAM lParam)
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
	        p_clickRecorder->save_delay();
            break;
        default:
            break;
        }
    }
    return CallNextHookEx(hook, nCode, wParam, lParam);
}

void MouseHook::Thread()
{
    //init hook
    //LOG_DEBUG("hooking mouse");
    hook = SetWindowsHookEx(WH_MOUSE_LL, mousecallback, NULL, 0);
    //hook ? LOG_DEBUG("hook successful") : LOG_ERROR("failed to set hook");

    while (toad::is_running)
    {
        if (GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }       
    }
}

void MouseHook::Unhook()
{
#ifndef _DEBUG
    LOG_DEBUG("Unhooking mouse");
    //Unhook
    UnhookWindowsHookEx(hook);
#endif
}
