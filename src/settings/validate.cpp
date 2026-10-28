#include "validate.h"
#include "validationHelpers.h"
#include <JSL.h>
#include <thread>
using validation::Errors;
using validation::failIf;
using validation::warnIf;

// Each section gets its own validateX function; add a call to it in validateSettings at the bottom.
// For checks that depend on another section, pass the whole MainSettings instead of just the sub-struct.
namespace
{
	void validateRuntime(const RuntimeSettings &s, Errors &errors)
	{
		// ---------- PARALLEL ---------
		const size_t hw = std::thread::hardware_concurrency();

		failIf(errors, s.ParallelThreads == 0, "ParallelThreads must be at least 1");
		if (hw > 0) // 0 means "unknown", so skip the upper-bound check
		{
			failIf(errors, s.ParallelThreads > hw, "ParallelThreads (", s.ParallelThreads, ") exceeds the hardware maximum (", hw, ")");
		}

		// ---------- VERBOSITY -------------
		// The hardcoded 'verbose' works for all combinations, because the only combination where its not the winner is when Silent wins, in which case this error is suppressed!
		warnIf(s.Quiet + s.Silent + s.Verbose > 1, "More than one of Quiet/Silent/Verbose is set; Verbose takes precedence");

		// ---------- INTERACTIVITY ---------
		// Piped stdin makes interactive mode impossible (error); a redirected stdout just means the user might not see the prompts (warning)
		if (s.Interactive)
		{
			failIf(errors, JSL::IO::Pipe::IsPiped(), "Interactive mode requires keyboard input, but stdin is piped or redirected");
			warnIf(!JSL::Display::Terminal().IsANSICapable(), "Interactive mode with stdout not connected to a terminal: prompts may not be visible");
		}
	}
} // namespace

void validateSettings(const MainSettings &target)
{
	Errors errors;

	validateRuntime(target.Runtime, errors);

	validation::throwIfAny(errors);
}
