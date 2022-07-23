#pragma once

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#include <tchar.h>
#include <toad.h>

#define WINDOW_WIDTH 700
#define WINDOW_HEIGHT 400

namespace toad {
	class Application
	{
	private:

		HWND				     hwnd = {};
		RECT					 rect = {};
		WNDCLASSEX				 wc = {};

		ImGuiIO* io;
		ImGuiStyle* style;

		const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		FILE* f;

		static bool CreateDeviceD3D(HWND hWnd);
		static void CleanupDeviceD3D();
		static void ResetDevice();
		static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		void InitConsole();
		bool SetupMenu();
		void MenuLoop();
		void UpdateCursorInfo();
	public:
		bool Init();
		void Run();
		void Dispose();
	};
}

