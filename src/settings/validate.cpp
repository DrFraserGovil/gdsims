#include "validate.h"
#include "validationHelpers.h"
#include <JSL.h>
#include <stdexcept>
#include <thread>
using Validation::Recorder;

// Each section gets its own validateX function; add a call to it in querySettingsConstraints below.
// For checks that depend on another section, pass the whole MainSettings instead of just the sub-struct.
// Severity: FatalIf if the program can't run at all, FailIf if the user could fix it from the interactive menu, WarnIf otherwise.
namespace
{
	void validateRuntime(const RuntimeSettings &s, Recorder &record)
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
		// Piped stdin makes interactive mode impossible (fatal: the menu can't fix it); a redirected stdout just means the user might not see the prompts (warning)
		if (s.Interactive)
		{
			record.FatalIf(JSL::IO::Pipe::IsPiped(), "Interactive mode requires keyboard input, but stdin is piped or redirected");
			record.WarnIf(!JSL::Display::Terminal().IsANSICapable(), "Interactive mode with stdout not connected to a terminal: prompts may not be visible");
		}
	}
} // namespace

void Validation::Status::LogWarnings() const
{
	if (!Warnings.empty())
	{
		LOG(WARN) << constructMessage(Warnings, "warning");
	}
}

void Validation::Status::LogErrors() const
{
	if (!Errors.empty())
	{
		LOG(ERROR) << constructMessage(Errors, "error");
	}
}

void Validation::Status::ThrowIfFatal() const
{
	if (!Fatal.empty())
	{
		throw std::invalid_argument(constructMessage(Fatal, "fatal error"));
	}
}

void Validation::Status::ThrowIfInvalid() const
{
	if (Valid())
	{
		return;
	}

	std::string msg = constructMessage(Fatal, "fatal error");
	if (!Fatal.empty() && !Errors.empty())
	{
		msg += "\n";
	}
	msg += constructMessage(Errors, "error");
	throw std::invalid_argument(msg);
}

Validation::Status querySettingsConstraints(const MainSettings &target)
{
	Recorder record;

	validateRuntime(target.Runtime, record);

	return record; // slices down to the Status (implicitly moved)
}

void validateSettings(const MainSettings &target)
{
	const auto status = querySettingsConstraints(target);

	status.LogWarnings();
	status.ThrowIfInvalid();
}
