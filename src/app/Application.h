#pragma once

#include <d3d9.h>
#include <tchar.h>
#include <toad.h>

#include "ImGuiWindow.h"
#include "Logger.h"

namespace toad {

class Application
{
public:
	Application();
	~Application();

	bool Init();
	void Run();

	ImGuiWindow& GetWindow();

	static logger& GetLogger();

private:
	void UpdateCursorInfo();

	ImGuiWindow m_window { "toadclicker", 400, 500 };
};

}

