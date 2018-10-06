#include <gtest/gtest.h>
#include <metl.h>

using namespace metl;

class UntypedValueFixture: public ::testing::Test
{
public:
	int i = 0;
};


TEST_F(UntypedValueFixture, construct)
{
	metl::UntypedValue<bool, int, double>{&i};
	metl::UntypedValue<bool, int, double>{i};
}

class UntypedValueVariableFixture: public UntypedValueFixture
{
public:
	metl::UntypedValue<bool, int, double> variable{&i};
};

TEST_F(UntypedValueVariableFixture, isConstant)
{
	ASSERT_FALSE(variable.isConstant());
}

TEST_F(UntypedValueVariableFixture, isVariable)
{
	ASSERT_TRUE(variable.isVariable());
}

TEST_F(UntypedValueVariableFixture, setValue)
{
	const auto oldI = i;
	const auto newI = i + 2;
	ASSERT_NO_THROW(variable.setValue(newI));

	ASSERT_NE(oldI, i);
	ASSERT_EQ(newI, i);

	const auto expr = variable.makeUntypedExpression();

	ASSERT_EQ(newI, expr.get<int>()());
	ASSERT_NE(oldI, expr.get<int>()());
	// variable.setValue(2.0); gives compiler warning for possible  loss of data. no clue how to put that into an assertion
}

TEST_F(UntypedValueVariableFixture, makeUntypedExpression)
{
	const auto expr = variable.makeUntypedExpression();

	const auto typedExpr = expr.get<int>();

	ASSERT_EQ(i, typedExpr());

	const auto oldI = i;
	i = i+2;
	ASSERT_EQ(i, typedExpr());
	ASSERT_NE(oldI, typedExpr());
}

TEST_F(UntypedValueVariableFixture, type)
{
	const auto expr = variable.makeUntypedExpression();

	std::cout << static_cast<int>(expr.type()) << std::endl;
	std::cout << static_cast<int>(variable.type()) << std::endl;

	ASSERT_EQ(expr.type(), variable.type());
}

class UntypedValueConstantFixture: public UntypedValueFixture
{
public:
	metl::UntypedValue<bool, int, double> constant{i};
};

TEST_F(UntypedValueConstantFixture, isConstant)
{
	ASSERT_TRUE(constant.isConstant());
}

TEST_F(UntypedValueConstantFixture, isVariable)
{
	ASSERT_FALSE(constant.isVariable());
}

TEST_F(UntypedValueConstantFixture, setValue)
{
	const auto oldI = i;
	const auto newI = i+2;
	ASSERT_NO_THROW(constant.setValue(newI));

	ASSERT_EQ(oldI, i);
	ASSERT_NE(newI, i);

	const auto expr = constant.makeUntypedExpression();

	ASSERT_EQ(newI, expr.get<int>()());
	ASSERT_NE(oldI, expr.get<int>()());
}

TEST_F(UntypedValueConstantFixture, makeUntypedExpression)
{
	const auto expr = constant.makeUntypedExpression();

	const auto typedExpr = expr.get<int>();

	ASSERT_EQ(i, typedExpr());

	const auto oldI = i;
	i = i+2;
	ASSERT_NE(i, typedExpr());
	ASSERT_EQ(oldI, typedExpr());
}

TEST_F(UntypedValueConstantFixture, type)
{
	const auto expr = constant.makeUntypedExpression();

	ASSERT_EQ(expr.type(), constant.type());
}