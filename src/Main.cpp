#include "pch.h"
#include "Application.h"
#include "Toad.h"

#ifdef _DEBUG
int main()
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
#endif
{
	toad::Application application;

	if (!application.Init())
	{
		LOG_ERROR("Failed to initialize");
		return 1;
	}

	application.Run();

	return 0;
}