#include "pch.h"
#include "Application.h"
#include "toad.h"

int main()
{
	toad::Application* application = new toad::Application();

	if (!application->Init())
	{
		log_error("Failed to initialize");
		delete application;
		return 1;
	}
	application->Run();
	application->Dispose();
	delete application;

	return 0;
}