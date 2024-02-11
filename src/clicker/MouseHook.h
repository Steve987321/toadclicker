#pragma once
#include <iostream>
#include <Windows.h>

class MouseHook
{
public:
    void Thread();
    void Unhook();
private:
	MSG m_msg;
};

inline auto p_mouseHook = std::make_unique<MouseHook>();
