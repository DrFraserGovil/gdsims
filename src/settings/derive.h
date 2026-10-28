#pragma once
#include "MainSettings.h"

//! @brief Updates any derived values (logging configuration etc.) following an update to the global Settings
//! @details Must be called every time the Settings are changed (after parsing, after the configuration menu, ...)
void deriveSettingsValues();
