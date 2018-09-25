
#include <gtest/gtest.h>
#include <complex>

#include "metl.h"

class VarExpressionFixture : public ::testing::Test
{
public:
	metl::VarExpression<bool, int, double> expression{ metl::exprType<int>([]()->int {return 0; }), metl::CATEGORY::CONSTEXPR };
};

TEST_F(VarExpressionFixture, type)
{
	ASSERT_EQ(expression.type(), expression.toType<int>());
}

TEST_F(VarExpressionFixture, category)
{
	ASSERT_EQ(expression.category(), metl::CATEGORY::CONSTEXPR);
}

TEST_F(VarExpressionFixture, getCorrect)
{
	ASSERT_EQ(expression.get<int>()(), 0);
}

TEST_F(VarExpressionFixture, getWrong)
{
	ASSERT_ANY_THROW(expression.get<bool>());
}
