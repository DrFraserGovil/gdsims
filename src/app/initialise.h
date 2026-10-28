#pragma once

enum class Mode
{
	Run,
	Configure,
	Error,
};

//! @brief Perform the initialisation steps for the settings, including a call to validate
Mode readConfiguration(int argc, char **argv);
