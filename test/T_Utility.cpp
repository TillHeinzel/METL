
#include <gtest/gtest.h>
#include <complex>

#include "metl.h"

using namespace metl;


class UtilityFixture : public ::testing::Test
{
public:
};

TEST_F(UtilityFixture, is_specialization_of)
{
	using Tup = std::tuple<int, bool, double>;

	static_assert(internal::is_specialization_of<Tup, std::tuple>::value, "");
	static_assert(!internal::is_specialization_of<Tup, UntypedExpression>::value, "");

	using Expr = UntypedExpression<int, bool, double>;

	static_assert(!internal::is_specialization_of<Expr, std::tuple>::value, "");
	static_assert(internal::is_specialization_of<Expr, UntypedExpression>::value, "");
	
}
