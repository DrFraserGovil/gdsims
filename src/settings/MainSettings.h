//! LSJ target
#pragma once
#include "settings/RuntimeSettings.h"
#include "settings/SimulationSettings.h"
#include <JSL/Interface.h>

class SettingObject : public JSL::Interface::Aggregator<SettingObject>
{
  public:
	RuntimeSettings Runtime;
	SimulationSettings Simulation;
#include "MainSettings.SettingObject.autogen"
};

extern const SettingObject &Settings;

//! @brief Gives access to the underlying datastructure of the global Settings, allowing them to be mutated. USE WITH CAUTION!
SettingObject &ChangeSettings();
