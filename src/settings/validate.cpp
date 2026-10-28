#include "validate.h"
#include "validationHelpers.h"
#include <JSL.h>
#include <thread>
using validation::QueuedMsg;

// Each section gets its own validateX function; add a call to it in validateSettings at the bottom.
// For checks that depend on another section, pass the whole MainSettings instead of just the sub-struct.
namespace
{
	void validateRuntime(const RuntimeSettings &s, validation::Recorder &record)
	{
		// ---------- PARALLEL ---------
		const size_t hw = std::thread::hardware_concurrency();

		record.FailIf(s.ParallelThreads == 0, "ParallelThreads must be at least 1");
		if (hw > 0) // 0 means "unknown", so skip the upper-bound check
		{
			record.FailIf(s.ParallelThreads > hw, "ParallelThreads (", s.ParallelThreads, ") exceeds the hardware maximum (", hw, ")");
		}

		// ---------- VERBOSITY -------------
		// The hardcoded 'verbose' works for all combinations, because the only combination where its not the winner is when Silent wins, in which case this error is suppressed!
		record.WarnIf(s.Quiet + s.Silent + s.Verbose > 1, "More than one of Quiet/Silent/Verbose is set; Verbose takes precedence");

		// ---------- INTERACTIVITY ---------
		// Piped stdin makes interactive mode impossible (error); a redirected stdout just means the user might not see the prompts (warning)
		if (s.Interactive)
		{
			record.FailIf(JSL::IO::Pipe::IsPiped(), "Interactive mode requires keyboard input, but stdin is piped or redirected");
			record.WarnIf(!JSL::Display::Terminal().IsANSICapable(), "Interactive mode with stdout not connected to a terminal: prompts may not be visible");
		}
	}
} // namespace

validation::Status querySettingsConstraints(const MainSettings &target)
{
	validation::Recorder record;

	validateRuntime(target.Runtime, record);

	return {record.Valid, record.Errors, record.Warnings};
}

void validateSettings(const MainSettings &target)
{
	auto status = querySettingsConstraints(target);

	// Interactive mode is different; we allow an invalid startup (but will refuse to start a sim until the validation
	validation::warnIfAny(status.Warnings);
	validation::throwIfAny(status.Errors, target.Runtime.Interactive);
}
