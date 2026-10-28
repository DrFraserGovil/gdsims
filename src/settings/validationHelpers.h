#pragma once
#include <JSL/Log/Core.h>
#include <JSL/Strings/MakeFrom.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// Machinery for validate.cpp: lets each check be written as a single failIf(...) line.
namespace validation
{
	using Errors = std::vector<std::string>;

	// Records an error message if the condition is true. The message is the concatenation of all parts, each stringified via JSL's makeFrom (avoids std::format, which is unreliable on Apple Clang).
	template <JSL::String::CanMakeFrom... Parts>
	void failIf(Errors &errors, bool condition, const Parts &...parts)
	{
		if (condition)
		{
			errors.push_back((std::string{} + ... + JSL::String::makeFrom(parts)));
		}
	}

	// Logs a warning (but doesn't fail validation) if the condition is true. Message parts are joined as in failIf.
	template <JSL::String::CanMakeFrom... Parts>
	void warnIf(bool condition, const Parts &...parts)
	{
		if (condition)
		{
			LOG(WARN) << (std::string{} + ... + JSL::String::makeFrom(parts));
		}
	}

	// Throws a single std::invalid_argument listing every collected error (numbered if there is more than one).
	inline void throwIfAny(const Errors &errors)
	{
		if (errors.empty())
		{
			return;
		}

		const size_t n = errors.size();
		std::ostringstream msg;
		msg << "Settings-Validation failed " << n << " check" << (n > 1 ? "s" : "") << "\n";
		for (size_t e = 0; e < n; ++e)
		{
			msg << "\t";
			if (n > 1)
			{
				msg << e + 1 << ") ";
			}
			msg << errors[e] << "\n";
		}
		throw std::invalid_argument(msg.str());
	}
}
