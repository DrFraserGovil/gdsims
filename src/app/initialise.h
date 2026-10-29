#pragma once

enum class Mode
{
	Run,
	Interactive,
	Version,
};

//! @brief Perform the initialisation steps for the settings, including validation
//! @details How strictly the settings are validated depends on the mode: Run is strict (throws on any error), Interactive only throws on fatal errors (other errors are logged, to be fixed from the menu), and informational modes such as Version skip validation entirely.
//! @returns The mode selected by the command given on the command line (Run if none was given)
Mode readConfiguration(int argc, char **argv);
