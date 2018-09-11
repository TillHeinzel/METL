
#include <gtest/gtest.h>
#include <complex>

#include "metl.h"
#include "src/SubExpression.h"

class SubExpressionFixture : public ::testing::Test
{
public:
	metl::SubExpression<bool, int, double> expression{ metl::exprType<int>([]()->int {return 0; }), metl::CATEGORY::CONSTEXPR };
};

TEST_F(SubExpressionFixture, type)
{
	ASSERT_EQ(expression.type(), expression.toType<int>());
}

TEST_F(SubExpressionFixture, category)
{
	ASSERT_EQ(expression.category(), metl::CATEGORY::CONSTEXPR);
}

TEST_F(SubExpressionFixture, setGetCorrect)
{
	ASSERT_EQ(expression.get<int>()(), 0);
}

TEST_F(SubExpressionFixture, setGetWrong)
{
	ASSERT_ANY_THROW(expression.get<bool>());
}
