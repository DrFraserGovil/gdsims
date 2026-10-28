#include "settings/MainSettings.h"
#include "settings/validate.h"
#include <JSL.h>

void ConfigureLogging()
{
	JSL::Log::Global().AlignSize(40);
	JSL::Log::Global().Level = INFO;
	JSL::Log::Global().ShowHeaders = false;
	JSL::Log::Global().DebugColour = JSL::Display::Colour(60, 60, 130);
	JSL::Log::Global().AlignSize(30);

	if (Settings.Runtime.Verbose)
	{
		JSL::Log::Global().Level = DEBUG;
		JSL::Log::Global().ShowHeaders = true;
	}
	else if (Settings.Runtime.Silent)
	{
		JSL::Log::Global().Level = ERROR;
	}
	else if (Settings.Runtime.Quiet)
	{
		JSL::Log::Global().Level = WARN;
	}
}

int main(int argc, char **argv)
{
	try
	{
		ChangeSettings().Parse(argc, argv);
		ConfigureLogging();

		validateSettings(Settings);
	}
	catch (std::exception &e)
	{
		LOG(ERROR) << "FATAL ERROR: \n"
				   << e.what();
		LOG(INFO) << "GDSiMS cannot recover: exiting with code 1";
		return 1;
	}

	return 0;
}
