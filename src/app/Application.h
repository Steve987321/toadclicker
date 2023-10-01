#pragma once

#include <d3d9.h>
#include <tchar.h>
#include <toad.h>

#include "ImGuiWindow.h"

#define WINDOW_WIDTH 700
#define WINDOW_HEIGHT 400

namespace toad {

class Application
{
public:
	Application();
	~Application();

	bool Init();
	void Run();
	void Dispose();

	ImGuiWindow& GetWindow();

private:
	FILE* m_f;

	void InitConsole();
	void MenuLoop();
	void UpdateCursorInfo();

	ImGuiWindow m_window { "toadclicker", 400, 700 };
};

}

