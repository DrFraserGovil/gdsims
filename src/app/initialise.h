#pragma once

enum class Mode
{
	Run,
	Interactive,
};

//! @brief Perform the initialisation steps for the settings, including validation
//! @details Non-interactive runs are validated strictly (throws on any error). Interactive runs only throw on fatal errors; other errors are logged, to be fixed from the menu.
//! @returns Run for a non-interactive run, Configure for an interactive one
Mode readConfiguration(int argc, char **argv);
