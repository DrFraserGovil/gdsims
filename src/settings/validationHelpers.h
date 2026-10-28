#pragma once
#include <JSL/Log.h>
#include <JSL/Strings/MakeFrom.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// Machinery for validate.cpp: lets each check be written as a single failIf(...) line.
namespace validation
{
	using QueuedMsg = std::vector<std::string>;

	class Recorder
	{
	  public:
		bool Valid = true;
		std::vector<std::string> Errors;
		std::vector<std::string> Warnings;

		// Records an error message if the condition is true. The message is the concatenation of all parts, each stringified via JSL's makeFrom (avoids std::format, which is unreliable on Apple Clang).
		template <JSL::String::CanMakeFrom... Parts>
		void FailIf(bool condition, const Parts &...parts)
		{
			if (condition)
			{
				Valid = false;
				Errors.push_back((std::string{} + ... + JSL::String::makeFrom(parts)));
			}
		}
		// Logs a warning (but doesn't fail validation) if the condition is true. Message parts are joined as in failIf.
		template <JSL::String::CanMakeFrom... Parts>
		void WarnIf(bool condition, const Parts &...parts)
		{
			if (condition)
			{
				Warnings.push_back((std::string{} + ... + JSL::String::makeFrom(parts)));
			}
		}
	};

	inline std::string constructMessage(const QueuedMsg &msg, const std::string type)
	{
		const size_t n = msg.size();
		std::ostringstream os;
		os << "Settings-Validation gave " << n << " " << type << (n > 1 ? "s" : "") << "\n";
		for (size_t e = 0; e < n; ++e)
		{
			os << "\t";
			if (n > 1)
			{
				os << e + 1 << ") ";
			}
			os << msg[e] << "\n";
		}
		return os.str();
	}

	// Throws a single std::invalid_argument listing every collected error (numbered if there is more than one).
	inline void throwIfAny(const QueuedMsg &errors, bool logNotThrow)
	{
		if (errors.empty())
		{
			return;
		}

		auto msg = constructMessage(errors, "error");
		if (logNotThrow)
		{
			LOG(ERROR) << msg;
			LOG(INFO) << "These must be fixed in interactive mode before a simulation can be launched";
		}
		else
		{
			throw std::invalid_argument(msg);
		}
	}

	inline void warnIfAny(const QueuedMsg &warnings)
	{
		if (warnings.empty()) return;

		LOG(WARN) << constructMessage(warnings, "error");
	}
} // namespace validation
