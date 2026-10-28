#pragma once
#include "MainSettings.h"
#include <string>
#include <vector>

namespace validation
{
	//! @brief The outcome of a settings validation, split by severity
	//! @details Fatal: the program cannot run at all. Errors: a simulation cannot be launched, but (in interactive mode) the settings can still be fixed. Warnings: informational only.
	struct Status
	{
		std::vector<std::string> Fatal;
		std::vector<std::string> Errors;
		std::vector<std::string> Warnings;

		//! @returns true if there are no fatal errors or errors (a simulation may be launched)
		bool Valid() const { return Fatal.empty() && Errors.empty(); }

		//! @brief Logs all warnings as a single WARN message (does nothing if there are none)
		void LogWarnings() const;

		//! @brief Logs all (non-fatal) errors as a single ERROR message (does nothing if there are none)
		void LogErrors() const;

		//! @brief Throws a std::invalid_argument listing the fatal errors, if there are any
		void ThrowIfFatal() const;

		//! @brief Throws a std::invalid_argument listing the fatal errors and errors, if there are any
		void ThrowIfInvalid() const;
	};
} // namespace validation

//! @brief Collects a status object for the provided settings object, but does not throw or print anything
//! @param target a full-suite settings object to be queried
//! @returns A status object detailing any fatal errors, errors or warnings
validation::Status querySettingsConstraints(const MainSettings &target);

//! @brief Strict validation: equivalent to calling querySettingsConstraints, then LogWarnings and ThrowIfInvalid on the result
//! @details Throws if target has any fatal errors or errors (and logs warnings first)
//! @param target a full-suite settings object to be queried
void validateSettings(const MainSettings &target);
