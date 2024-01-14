#include "pch.h"
#include "Application.h"
#include "toad.h"

int main()
{
	toad::Application application {};

	if (!application.Init())
	{
		log_error("Failed to initialize");
		return 1;
	}
	application.Run();
	application.Dispose();

	return 0;
}