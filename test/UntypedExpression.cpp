
#include <gtest/gtest.h>
#include <complex>

#include "metl.h"

class UntypedExpressionFixture : public ::testing::Test
{
public:
	metl::TypedExpression<int> function{ []()->int {return 0; } };

	using Expression = metl::UntypedExpression<bool, int, double>;
	Expression expression = Expression::makeConstexpr( function());
};

TEST_F(UntypedExpressionFixture, type)
{
	EXPECT_EQ(expression.type(), expression.toType<int>());
}

TEST_F(UntypedExpressionFixture, isType)
{
	EXPECT_TRUE(expression.isType<int>());

	EXPECT_FALSE(expression.isType<double>());
	EXPECT_FALSE(expression.isType<bool>());
}

TEST_F(UntypedExpressionFixture, category)
{
	EXPECT_EQ(expression.category(), metl::CATEGORY::CONSTEXPR);
}

TEST_F(UntypedExpressionFixture, getCorrect)
{
	EXPECT_EQ(expression.get<int>()(), 0);
}

TEST_F(UntypedExpressionFixture, getWrong)
{
	EXPECT_ANY_THROW(expression.get<bool>());
}

TEST_F(UntypedExpressionFixture, copyConstruct)
{
	auto expression2 = expression;

	EXPECT_EQ(expression2.get<int>()(), 0);
}

TEST_F(UntypedExpressionFixture, moveConstruct)
{
	auto expression2 = expression;
	auto expression3 = std::move(expression2);

	EXPECT_EQ(expression3.get<int>()(), 0);
}

TEST_F(UntypedExpressionFixture, evaluatedExpression)
{
	int evaluationCount = 0;

	metl::TypedExpression<int> function2{ [&evaluationCount]()->int
	{
		++evaluationCount;
		return 0;
	} };
	
	auto expression2 = Expression::makeNonConstexpr(function2);

	EXPECT_EQ(0, evaluationCount);

	auto evaluatedExpression = expression2.evaluatedExpression();

	EXPECT_EQ(1, evaluationCount);

	EXPECT_EQ(0, evaluatedExpression.get<int>()());

	EXPECT_EQ(1, evaluationCount);
}