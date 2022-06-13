#pragma once
#include <iostream>
#include <Windows.h>

class c_slotWhitelist
{
private:
    MSG msg;
public:
    void thread();
};

inline auto p_slotWhitelist = std::make_unique<c_slotWhitelist>();
