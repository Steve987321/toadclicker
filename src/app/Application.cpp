#include "pch.h"
#include "toad.h"
#include "Application.h"

namespace toad {
    Application::Application()
    {
        m_window.SetUI(renderUI());
    }

    Application::~Application()
    {
    }

    void Application::InitConsole()
    {
        AllocConsole();
        freopen_s(&m_f,"CONOUT$", "w", stdout);
    }

    void Application::UpdateCursorInfo()
    {
        // get cursor info
        CURSORINFO ci{ sizeof(CURSORINFO) };
        if (GetCursorInfo(&ci))
        {
            auto handle = ci.hCursor;

            toad::clicker::cursor_visible = int(handle) > 50000 & (int(handle) < 100000);
        }
    }

    bool Application::Init()
    {
//#ifdef _DEBUG
        InitConsole();
//#else
//      ShowWindow(GetConsoleWindow(), SW_HIDE);
//#endif 
        if (!toad::init_toad()) { log_error("failed to initialize toadclicker"); return false; }

        ShowWindow(GetConsoleWindow(), SW_HIDE);
        return true;
    }

    void Application::Run()
    {
        MenuLoop();
    }

    void Application::Dispose()
    {
        fclose(m_f);
        FreeConsole();

        toad::is_running = false;

        p_clicker->stop_thread();
        p_right_clicker->stop_thread();
        p_doubleClicker->stop_thread();
        p_mouseHook->unhook();
    }

    ImGuiWindow& Application::GetWindow()
    {
        return m_window;
    }

}