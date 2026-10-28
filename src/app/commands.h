#pragma once

// Gateway functions: the single entry point for each top-level action, shared by the command line (main.cpp) and the interactive menu (menu.cpp)

//! @brief Launches a simulation using the current global Settings
//! @details Throws if the Settings are invalid, whichever route was used to get here
void runSimulation();

//! @brief Prints the version number to stdout (unformatted, so that scripts can capture it)
void printVersion();

//! @brief PLACEHOLDER: not yet implemented (throws)
void dump();
