#include "pch.h"
#include "toad.h"
#include "Application.h"

namespace toad {
    Application::Application()
    {
        m_window.SetUI(renderUI);
        m_window.StartWindow();
    }

    Application::~Application()
    {
        toad::is_running = false;

        m_window.DestroyWindow();

        p_clicker->stop_thread();
        p_right_clicker->stop_thread();
        p_doubleClicker->stop_thread();
        p_mouseHook->unhook();
    }

    void Application::UpdateCursorInfo()
    {
        // get cursor info
        CURSORINFO ci{ sizeof(CURSORINFO) };
        if (GetCursorInfo(&ci))
        {
            auto handle = ci.hCursor;

            toad::clicker::cursor_visible = (int)handle > 50000 && ((int)handle < 100000);
            log_debugf("Cursor showing test: %d", ci.flags & CURSOR_SHOWING);
        }
    }

    bool Application::Init()
    {
#ifndef _DEBUG
        ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
        if (!toad::init_toad()) 
        {
            log_error("Failed to initialize toadclicker");
            return false;
        }

        ShowWindow(GetConsoleWindow(), SW_HIDE);
        return true;
    }

    void Application::Run()
    {
        while (m_window.IsActive())
        {
            // TODO: UpdateCursorInfo();
        }
    }

    ImGuiWindow& Application::GetWindow()
    {
        return m_window;
    }

}