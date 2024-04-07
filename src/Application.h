#pragma once

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#include <tchar.h>
#include <toad.h>

#define WINDOW_WIDTH 700
#define WINDOW_HEIGHT 400

namespace toad 
{
	// Handles the window and main application
	class Application
	{
	public:
		Application();
		~Application();

	public:
		bool Init();
		void Run();
		void Dispose();

		Application& Get();

	private:
		bool SetupMenu();
		void UpdateCursorInfo();

		static bool CreateDeviceD3D(HWND hWnd);
		static void CleanupDeviceD3D();
		static void ResetDevice();
		static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		inline static Application* s_instance = nullptr;

		HWND m_hwnd = {};
		RECT m_rect = {};
		WNDCLASSEX m_wc = {};

		ImGuiIO* m_io = nullptr;
		ImGuiStyle* m_style = nullptr;

		const ImVec4 m_clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	};

}

