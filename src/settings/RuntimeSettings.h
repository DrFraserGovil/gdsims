//! LSJ target
#pragma once
#include <JSL/Interface.h>
class RuntimeSettings : public JSL::Interface::Aggregator<RuntimeSettings>
{
  public:
	//! @alias s, silent
	//! @brief If true, sets the logging to Silent mode (only errors). Overrides Quiet mode.
	bool Silent = false;

	//! @alias q, quiet
	//! @brief If true, sets the logging to Quiet mode (only warnings and errors)
	bool Quiet = false;

	//! @alias v, verbose
	//! @brief If true, sets the logging to Loud mode (includes debug messages). Overrides Quiet and Silent modes.
	bool Verbose = false;

	//! @alias thread
	//! @brief The number of threads to run the simulation suite on.
	size_t ParallelThreads = 1;

	//! @alias i, interface, interactive
	//! @brief Activates the interactive menu option for configuring runs
	bool Interactive = false;

#include "RuntimeSettings.RuntimeSettings.autogen"
};
