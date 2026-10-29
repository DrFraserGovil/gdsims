#include "settings/validate.h"
#include <catch2/catch_test_macros.hpp>
#include <stdexcept>
#include <thread>

// The Interactive checks depend on whether stdin/stdout are terminals, so they are left out here (they would need JSL's pipe spoofing)

TEST_CASE("Default settings are valid", "[validation]")
{
	const MainSettings s;
	const auto status = querySettingsConstraints(s);

	REQUIRE(status.Valid());
	REQUIRE(status.Fatal.empty());
	REQUIRE(status.Errors.empty());
	REQUIRE(status.Warnings.empty());
}

TEST_CASE("ParallelThreads must be within hardware limits", "[validation]")
{
	MainSettings s;

	SECTION("Zero threads is an error")
	{
		s.Runtime.ParallelThreads = 0;
		const auto status = querySettingsConstraints(s);
		REQUIRE_FALSE(status.Valid());
		REQUIRE(status.Errors.size() == 1);
		REQUIRE(status.Fatal.empty());
	}

	SECTION("More threads than the hardware supports is an error")
	{
		const size_t hw = std::thread::hardware_concurrency();
		if (hw == 0)
		{
			SKIP("hardware_concurrency is unknown on this machine");
		}
		s.Runtime.ParallelThreads = hw + 1;
		REQUIRE_FALSE(querySettingsConstraints(s).Valid());
	}
}

TEST_CASE("Conflicting verbosity flags warn but stay valid", "[validation]")
{
	MainSettings s;
	s.Runtime.Quiet = true;
	s.Runtime.Verbose = true;

	const auto status = querySettingsConstraints(s);
	REQUIRE(status.Valid());
	REQUIRE(status.Warnings.size() == 1);
}

TEST_CASE("ThrowIfInvalid only throws for invalid settings", "[validation]")
{
	MainSettings s;
	REQUIRE_NOTHROW(querySettingsConstraints(s).ThrowIfInvalid());

	s.Runtime.ParallelThreads = 0;
	REQUIRE_THROWS_AS(querySettingsConstraints(s).ThrowIfInvalid(), std::invalid_argument);
}
