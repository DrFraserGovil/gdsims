#pragma once
#include "validate.h"
#include <JSL/Strings/MakeFrom.h>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

// Machinery for validate.cpp: lets each check be written as a single record.FailIf(...) line.
namespace Validation
{
	using QueuedMsg = std::vector<std::string>;

	// A Status which can be filled in one check at a time. Message parts are concatenated, each stringified via JSL's makeFrom (avoids std::format, which is unreliable on Apple Clang).
	class Recorder : public Status
	{
	  public:
		// Records a fatal error if the condition is true: the program cannot continue, even in interactive mode.
		template <JSL::String::CanMakeFrom... Parts>
		void FatalIf(bool condition, const Parts &...parts)
		{
			RecordIf(Fatal, condition, parts...);
		}

		// Records an error if the condition is true: a simulation cannot be launched until it is fixed.
		template <JSL::String::CanMakeFrom... Parts>
		void FailIf(bool condition, const Parts &...parts)
		{
			RecordIf(Errors, condition, parts...);
		}

		// Records a warning if the condition is true: doesn't fail validation.
		template <JSL::String::CanMakeFrom... Parts>
		void WarnIf(bool condition, const Parts &...parts)
		{
			RecordIf(Warnings, condition, parts...);
		}

	  private:
		template <JSL::String::CanMakeFrom... Parts>
		static void RecordIf(QueuedMsg &target, bool condition, const Parts &...parts)
		{
			if (condition)
			{
				target.push_back((std::string{} + ... + JSL::String::makeFrom(parts)));
			}
		}
	};

	// Formats a list of messages under a header line (numbered if there is more than one). Returns an empty string if there are no messages.
	inline std::string constructMessage(const QueuedMsg &msg, std::string_view type)
	{
		const size_t n = msg.size();
		if (n == 0)
		{
			return "";
		}

		std::ostringstream os;
		os << "Settings-Validation gave " << n << " " << type << (n > 1 ? "s" : "");
		for (size_t e = 0; e < n; ++e)
		{
			os << "\n\t";
			if (n > 1)
			{
				os << e + 1 << ") ";
			}
			os << msg[e];
		}
		return os.str();
	}
} // namespace Validation
