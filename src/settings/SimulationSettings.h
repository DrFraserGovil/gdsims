//! LSJ target
#pragma once
#include <JSL/Interface.h>
class SimulationSettings : public JSL::Interface::Aggregator<SimulationSettings>
{
  public:
	//! @alias T
	//! @brief The number of timesteps to run the simulation for
	int TimeSteps = 100;
#include "SimulationSettings.SimulationSettings.autogen"
};
