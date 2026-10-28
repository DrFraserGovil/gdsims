#include "menu.h"
#include "settings/derive.h"
#include "settings/validate.h"
#include <JSL.h>

void settingsMenu()
{
	LOG(INFO) << JSL::Display::Yellow(true) << "\t-- Configuration Interface --\t\t" << JSL::Display::ResetAll();
	ChangeSettings() = JSL::Interface::Console(Settings, {.ExitString = "save", .ConsoleString = "gdsims-config > "});

	deriveSettingsValues();
	auto status = querySettingsConstraints(Settings);

	if (!status.Valid())
	{
		LOG(WARN) << "The settings are an invalid state: a simulation will refuse to launch until this is rectified";
	}
	status.LogWarnings();
	status.LogErrors();
	status.ThrowIfFatal();

	// Reconfigure and reparse
}
void openMenu()
{
	settingsMenu();
}
