#include "pch.h"
#include "app/Application.h"
#include "toad.h"

using namespace toad;

int main()
{
	Application app;

	if (!app.Init())
	{
		LOGERROR("Failed to initialize");
		return 1;
	}
	app.Run();

	return 0;
}