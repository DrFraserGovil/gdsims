#include "commands.h"
#include "settings/validate.h"
#include "version.h"
#include <JSL/Log.h>
#include <iostream>
#include <stdexcept>

void runSimulation()
{
	// Silent check: warnings have already been reported whenever the settings last changed (startup, settings menu, run overrides)
	querySettingsConstraints(Settings).ThrowIfInvalid();

	LOG(INFO) << "Launching simulation";
	// TODO: construct the model and run it
}

void printVersion()
{
	std::cout << Version::String << "\n";
}

void dump()
{
	// TODO: decide what gets dumped (the resolved config via Aggregator::Export? model state?) and where it goes
	throw std::logic_error("dump is not yet implemented");
}
