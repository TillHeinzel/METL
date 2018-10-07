
#include <gtest/gtest.h>
#include <complex>

#include "metl.hpp"
#include "src/API/OutputExpression.hpp"

using namespace metl;

class OutExpressionFixture : public ::testing::Test
{
public:
	using Expr = UntypedExpression<bool, int, double>;

	Expr varExpression = Expr::makeConstexpr(0);

	std::vector<internal::UntypedConversion<Expr>> casts
	{
		{
			internal::makeDynamicConversion<Expr, int>([](int i) {return static_cast<double>(i); })
		}
	};

	OutputExpression<bool, int, double> expr{ varExpression , casts};
};

TEST_F(OutExpressionFixture, isType)
{
	ASSERT_TRUE(expr.isType<int>());

	ASSERT_FALSE(expr.isType<double>());
	ASSERT_FALSE(expr.isType<bool>());

	//ASSERT_FALSE(expr.isType<unsigned>()); // should not compile
	//ASSERT_FALSE(expr.isType<std::string>()); // should not compile
}

TEST_F(OutExpressionFixture, get)
{
	ASSERT_EQ(0, expr.get<int>()());
	ASSERT_EQ(0.0, expr.get<double>()());

	ASSERT_ANY_THROW(expr.get<bool>()());
}
