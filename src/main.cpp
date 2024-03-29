#include "pch.h"
#include "Application.h"
#include "toad.h"

int main()
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