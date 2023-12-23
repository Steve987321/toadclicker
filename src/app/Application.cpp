#include "pch.h"

#include "Application.h"

namespace toad {

    Application::Application()
    {
        m_window.SetUI(render_ui);
        m_window.StartWindow();
    }

    Application::~Application()
    {
        is_running = false;

        m_window.DestroyWindow();

        p_clicker->stop_thread();
        p_right_clicker->stop_thread();
        p_doubleClicker->stop_thread();
        p_mouseHook->unhook();
    }

    void Application::UpdateCursorInfo()
    {
        CURSORINFO ci{ sizeof(CURSORINFO) };
        if (GetCursorInfo(&ci))
        {
            auto handle = ci.hCursor;

            clicker::cursor_visible = (int)handle > 50000 && ((int)handle < 100000);
        }
    }

    bool Application::Init()
    {
        if (!init_toad()) 
        {
            LOGERROR("Failed to initialize toadclicker");
            return false;
        }

        return true;
    }

    void Application::Run()
    {
        while (m_window.IsActive())
        {
            UpdateCursorInfo();
            SLEEP(1);
        }
    }

    ImGuiWindow& Application::GetWindow()
    {
        return m_window;
    }

    Logger& Application::GetLogger()
    {
        static Logger s_logger;
        return s_logger;
    }

}