
#include <gtest/gtest.h>
#include <complex>

#include "src/OutputExpression.h"

using namespace metl;

class OutExpressionFixture : public ::testing::Test
{
public:
	using Expr = VarExpression<bool, int, double>;

	Expr varExpression{ exprType<int>([]()->int {return 0; }), CATEGORY::CONSTEXPR };

	std::map<std::string, internal::CastImpl<Expr>> casts
	{
		{
			internal::mangleCast(classToType2<int, bool, int, double>(), classToType2<double, bool, int, double>()),
			internal::makeCastImpl<Expr, int>([](int i) {return static_cast<double>(i); })
		}
	};

	OutputExpression<bool, int, double> expr{ varExpression , casts};
};

TEST_F(OutExpressionFixture, isType)
{
	ASSERT_TRUE(expr.isType<int>());

	ASSERT_FALSE(expr.isType<double>());
	ASSERT_FALSE(expr.isType<bool>());

	ASSERT_FALSE(expr.isType<unsigned>());
	ASSERT_FALSE(expr.isType<std::string>());
}

TEST_F(OutExpressionFixture, get)
{
	ASSERT_EQ(0, expr.get<int>()());
	ASSERT_EQ(0.0, expr.get<double>()());

	ASSERT_ANY_THROW(expr.get<bool>()());
}
