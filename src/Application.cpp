#include "pch.h"
#include "Application.h"

#include "GUI.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LPDIRECT3D9              g_pD3D;
static LPDIRECT3DDEVICE9        g_pd3dDevice;
static D3DPRESENT_PARAMETERS    g_d3dpp;

static void slot_update()
{
	if (toad::clicker::slot_whitelist && toad::clicker::enabled && toad::window_is_focused(toad::clicking_window)) {
		for (int i = 0; i < toad::hotbar_virtual_keycodes.size(); i++)
		{
			if (GetAsyncKeyState(toad::hotbar_virtual_keycodes[i]) & 1)
			{
				toad::clicker::curr_slot = i;
			}
		}
	}
}

namespace toad 
{
	Application::Application()
	{
		s_instance = this;
	}

	Application::~Application()
	{
		Dispose();
	}

	bool Application::Init()
	{
        LOG_DEBUGF("[Application] Init");

		if (!SetupMenu())
		{
			LOG_ERROR("[Application] Failed to setup Menu");
			return false;
		}
		if (!init_toad())
		{
			LOG_ERROR("[Application] Failed to initialize toadclicker");
			return false;
		}

		return true;
	}

	void Application::Run()
	{
		while (toad::is_running)
		{
			MSG msg;
			while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if (msg.message == WM_QUIT)
				{
					toad::is_running = false;
				}
			}

			if (!toad::is_running)
				break;

			UpdateCursorInfo();
            slot_update();

			// cpu 
            if (GetActiveWindow() == FindWindowA(NULL, "Toad.exe") && GetForegroundWindow() != toad::clicking_window)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

			// Start the Dear ImGui frame
			ImGui_ImplDX9_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			// ui
			toad::render_ui(m_hwnd);

			// Rendering
			ImGui::EndFrame();
			g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
			g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
			D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(m_clearColor.x * m_clearColor.w * 255.0f), (int)(m_clearColor.y * m_clearColor.w * 255.0f), (int)(m_clearColor.z * m_clearColor.w * 255.0f), (int)(m_clearColor.w * 255.0f));
			g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);

			if (g_pd3dDevice->BeginScene() >= 0)
			{
				ImGui::Render();
				ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
				g_pd3dDevice->EndScene();
			}

			// Update and Render additional Platform Windows
			if (m_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}

			HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

			// Handle loss of D3D9 device
			if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
				ResetDevice();
		}
	}

	void Application::Dispose()
	{
		toad::is_running = false;

		p_clicker->StopThread();
		p_rightClicker->StopThread();
		p_doubleClicker->StopThread();
		p_mouseHook->Unhook();

		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		CleanupDeviceD3D();
		::DestroyWindow(m_hwnd);
		::UnregisterClass(m_wc.lpszClassName, m_wc.hInstance);
	}

	Application& Application::Get()
	{
		return *s_instance;
	}

    bool Application::CreateDeviceD3D(HWND hWnd)
    {
        if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        {
            LOG_ERROR("[Application] Failed to create IDirect3D9 object");
            return false;
        }

        // Create the D3DDevice
        ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
        g_d3dpp.Windowed = TRUE;
        g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
        g_d3dpp.EnableAutoDepthStencil = TRUE;
        g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
        g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
        if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        {
            LOG_ERROR("[Application] Failed to create D3Ddevice");
            return false;
        }

        return true;
    }

    void Application::CleanupDeviceD3D()
    {
        if (g_pd3dDevice)
        { 
            g_pd3dDevice->Release();
            g_pd3dDevice = NULL;
        }
        if (g_pD3D) 
        {
            g_pD3D->Release();
            g_pD3D = NULL; 
        }
    }

    void Application::ResetDevice()
    {
        ImGui_ImplDX9_InvalidateDeviceObjects();
        HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
        if (hr == D3DERR_INVALIDCALL)
            IM_ASSERT(0);
        ImGui_ImplDX9_CreateDeviceObjects();
    }

    LRESULT WINAPI Application::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;

        switch (msg)
        {
        case WM_SIZE:
            if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
            {
                g_d3dpp.BackBufferWidth = LOWORD(lParam);
                g_d3dpp.BackBufferHeight = HIWORD(lParam);
                ResetDevice();
            }
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_DPICHANGED:
            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
            {
                //const int dpi = HIWORD(wParam);
                //printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
                const RECT* suggested_rect = (RECT*)lParam;
                SetWindowPos(hWnd, NULL, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
            }
            break;
        }

        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    bool Application::SetupMenu()
    {
        LOG_DEBUGF("[Application] Creating window");

        GetWindowRect(GetDesktopWindow(), &m_rect);
        auto x = float(m_rect.right - WINDOW_WIDTH) / 2.f;
        auto y = float(m_rect.bottom - WINDOW_HEIGHT) / 2.f;
        m_wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("Toad Minecraft"), NULL };
        RegisterClassEx(&m_wc);

        m_hwnd = CreateWindow(m_wc.lpszClassName, _T("Toad"), WS_OVERLAPPEDWINDOW, x, y, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, m_wc.hInstance, NULL);

        // Initialize Direct3D
        if (!CreateDeviceD3D(m_hwnd))
        {
			LOG_ERROR("[Application] Failed to create Direct3D Device");
            CleanupDeviceD3D();
            UnregisterClass(m_wc.lpszClassName, m_wc.hInstance);
            return false;
        }

        // Show the window
        ::ShowWindow(m_hwnd, SW_HIDE);
        ::UpdateWindow(m_hwnd);

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        m_io = &ImGui::GetIO(); (void)m_io;
        m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        m_io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        m_style = &ImGui::GetStyle();
        if (m_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            m_style->WindowRounding = 0.0f;
            m_style->Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        ImGui_ImplWin32_Init(m_hwnd);
        ImGui_ImplDX9_Init(g_pd3dDevice);

        return true;
    }

    void Application::UpdateCursorInfo()
    {
        // Get cursor info
        CURSORINFO ci{ sizeof(CURSORINFO) };
        if (GetCursorInfo(&ci))
        {
            auto handle = ci.hCursor;

            toad::clicker::cursor_visible = int(handle) > 50000 & (int(handle) < 100000);
        }
    }
}