#include "slotWhitelist.h"
#include "toad.h"
#include <TlHelp32.h>

HHOOK hook;
MSG msg;

void hotkey_listener_thread()
{
    while (toad::is_running) {
        if (toad::clicker::slot_whitelist && toad::clicker::enabled && toad::window_is_focused(toad::clicking_window)) {
            for (int i = 0; i < toad::hotbarVKCodes.size(); i++)
            {
                if (GetAsyncKeyState(toad::hotbarVKCodes[i]) & 1)
                {
                    toad::clicker::curr_slot = i;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        else std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

LRESULT _stdcall mousecallback(int nCode, WPARAM wParam, LPARAM lParam)
{
    MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;
    if (wParam == WM_MOUSEWHEEL) {
        if ((int)pMouseStruct->mouseData < 0) toad::clicker::curr_slot == 0 ? toad::clicker::curr_slot = 8 : toad::clicker::curr_slot--;
        else if ((int)pMouseStruct->mouseData > 0) toad::clicker::curr_slot == 8 ? toad::clicker::curr_slot = 0 : toad::clicker::curr_slot++;
    }
    return CallNextHookEx(hook, nCode, wParam, lParam);
}

void c_slotWhitelist::thread()
{
    //init hook
    log_debug("hooking mouse");
    hook = SetWindowsHookEx(WH_MOUSE_LL, mousecallback, NULL, 0);
    hook ? log_debug("hook successful") : log_error("failed to set hook");

    std::thread(hotkey_listener_thread).detach();

    while (toad::is_running)
    {
        if (GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }       
    }

    // TODO: this isn't getting called 

    log_debug("unhooking mouse");
    //unhook
    UnhookWindowsHookEx(hook);
}
