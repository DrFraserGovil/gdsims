#pragma once
#include "MainSettings.h"

namespace validation
{
	struct Status
	{
	  public:
		bool Valid;
		std::vector<std::string> Errors;
		std::vector<std::string> Warnings;
	};
} // namespace validation

//! @brief Collects a status object for the provided settings object, but does not throw or print anything
//! @param target a full-suite settings object to be queried
//! @returns A status object detailing if the settings object is valid, and any errors or warnings
validation::Status querySettingsConstraints(const MainSettings &target);

//! @brief Equivalent to calling querySettingsConstraints and then validateSettings on the return value
//! @details Throws if target is invalid (and prints warnings)
//! @param target a full-suite settings object to be queried
void validateSettings(const MainSettings &target);
