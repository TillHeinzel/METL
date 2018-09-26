
#include <gtest/gtest.h>
#include <complex>

#include "metl.h"

class VarExpressionFixture : public ::testing::Test
{
public:
	metl::TypedExpression<int> function{ []()->int {return 0; } };
	metl::VarExpression<bool, int, double> expression{ function, metl::CATEGORY::CONSTEXPR };
};

TEST_F(VarExpressionFixture, type)
{
	EXPECT_EQ(expression.type(), expression.toType<int>());
}

TEST_F(VarExpressionFixture, isType)
{
	EXPECT_TRUE(expression.isType<int>());

	EXPECT_FALSE(expression.isType<double>());
	EXPECT_FALSE(expression.isType<bool>());
}

TEST_F(VarExpressionFixture, category)
{
	EXPECT_EQ(expression.category(), metl::CATEGORY::CONSTEXPR);
}

TEST_F(VarExpressionFixture, getCorrect)
{
	EXPECT_EQ(expression.get<int>()(), 0);
}

TEST_F(VarExpressionFixture, getWrong)
{
	EXPECT_ANY_THROW(expression.get<bool>());
}

TEST_F(VarExpressionFixture, copyConstruct)
{
	auto expression2 = expression;

	EXPECT_EQ(expression2.get<int>()(), 0);
}

TEST_F(VarExpressionFixture, moveConstruct)
{
	auto expression2 = expression;
	auto expression3 = std::move(expression2);

	EXPECT_EQ(expression3.get<int>()(), 0);
}

TEST_F(VarExpressionFixture, evaluatedExpression)
{
	int evaluationCount = 0;

	metl::TypedExpression<int> function2{ [&evaluationCount]()->int
	{
		++evaluationCount;
		return 0;
	} };
	metl::VarExpression<bool, int, double> expression2{ function2, metl::CATEGORY::CONSTEXPR };

	EXPECT_EQ(0, evaluationCount);

	auto evaluatedExpression = expression2.evaluatedExpression();

	EXPECT_EQ(1, evaluationCount);

	EXPECT_EQ(0, evaluatedExpression.get<int>()());

	EXPECT_EQ(1, evaluationCount);
}