#include "toad.h"
#include <ShObjIdl.h>

DWORD dwRemove = WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
// This should be kept for reverse operation
DWORD dwStyle;
HMENU hMenu;
WINDOWPLACEMENT wp; 


void toad::misc::hide(HWND window) {
    ::ShowWindow(window, SW_MINIMIZE);
    ::ShowWindow(window, SW_HIDE);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}
void toad::misc::show(HWND window) {
    ::ShowWindow(window, SW_SHOWDEFAULT);
    ::ShowWindow(window, SW_SHOW);
    ::ShowWindow(window, SW_HIDE);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
