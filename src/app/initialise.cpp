#include "initialise.h"
#include "settings/MainSettings.h"
#include "settings/validate.h"
#include <JSL/Log.h>

namespace
{
	void initialiseLogging()
	{
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
} // namespace

Mode readConfiguration(int argc, char **argv)
{
	ChangeSettings().Parse(argc, argv);

	initialiseLogging();

	if (!Settings.Runtime.Interactive)
	{
		validateSettings(Settings);
		return Mode::Run;
	}

	// Interactive mode allows an invalid startup, as long as nothing is fatal: the menu must refuse to launch a simulation until the errors are fixed
	const auto status = querySettingsConstraints(Settings);
	status.LogWarnings();
	status.ThrowIfFatal();
	if (!status.Valid())
	{
		status.LogErrors();
		LOG(INFO) << "These must be fixed in interactive mode before a simulation can be launched";
	}

	return Mode::Interactive;
}
