#include "pch.h"

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
            //log_debugf("Cursor showing test: %d", toad::clicker::cursor_visible);
        }
    }

    bool Application::Init()
    {
        if (!toad::init_toad()) 
        {
            log_error("Failed to initialize toadclicker");
            return false;
        }

        return true;
    }

    void Application::Run()
    {
        while (m_window.IsActive())
        {
            UpdateCursorInfo();
        }
    }

    ImGuiWindow& Application::GetWindow()
    {
        return m_window;
    }

    logger& Application::GetLogger()
    {
        static logger s_log_instance;
        return s_log_instance;
    }

}