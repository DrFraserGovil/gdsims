#include <catch2/catch_test_macros.hpp>
#include "random.h"

TEST_CASE("random_discrete stays within its bounds", "[random]")
{
	for (int i = 0; i < 1000; ++i)
	{
		int x = random_discrete(3, 7);
		REQUIRE(x >= 3);
		REQUIRE(x <= 7);
	}
}

TEST_CASE("random_binomial handles degenerate probabilities", "[random]")
{
	REQUIRE(random_binomial(50, 0.0) == 0);
	REQUIRE(random_binomial(50, 1.0) == 50);
}
