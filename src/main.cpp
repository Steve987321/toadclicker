#include "pch.h"
#include "app/Application.h"
#include "toad.h"

using namespace toad;

int main()
{
	Application app;

	if (!app.Init())
	{
		log_error("Failed to initialize");
		return 1;
	}
	app.Run();
	app.Dispose();

	return 0;
}