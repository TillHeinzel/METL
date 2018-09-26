
#include <gtest/gtest.h>
#include <complex>

#include "src/OutputExpression.h"

class OutExpressionFixture : public ::testing::Test
{
public:
	using Expr = metl::VarExpression<bool, int, double>;

	Expr varExpression{ metl::exprType<int>([]()->int {return 0; }), metl::CATEGORY::CONSTEXPR };

	std::map<std::string, metl::internal::CastImpl<Expr>> casts{};

	metl::OutputExpression<bool, int, double> expr{ varExpression , casts};
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

	ASSERT_ANY_THROW(expr.get<double>()());
	ASSERT_ANY_THROW(expr.get<bool>()());
}
