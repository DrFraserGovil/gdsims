//! LSJ target
#pragma once
#include "settings/RuntimeSettings.h"
#include "settings/SimulationSettings.h"
#include <JSL/Interface.h>

class MainSettings: public JSL::Interface::Aggregator<MainSettings>
{
  public:
	RuntimeSettings Runtime;
	SimulationSettings Simulation;
	#include "MainSettings.MainSettings.autogen"
};

extern const MainSettings &Settings;

//! @brief Gives access to the underlying datastructure of the global Settings, allowing them to be mutated. USE WITH CAUTION!
MainSettings &ChangeSettings();
