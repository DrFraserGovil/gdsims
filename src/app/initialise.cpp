#include "initialise.h"
#include "settings/MainSettings.h"
#include "settings/validate.h"
#include <JSL/Log.h>

void initialiseLogging()
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

Mode readConfiguration(int argc, char **argv)
{
	ChangeSettings().Parse(argc, argv);

	initialiseLogging();
	LOG(INFO) << Settings.Runtime.ParallelThreads;
	validateSettings(Settings);

	return Mode::Run;
}
