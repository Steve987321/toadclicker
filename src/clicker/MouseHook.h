#pragma once
#include <iostream>
#include <Windows.h>

class c_mouseHook
{
private:
    MSG msg;
public:
    void thread();
};

inline auto p_mouseHook = std::make_unique<c_mouseHook>();
