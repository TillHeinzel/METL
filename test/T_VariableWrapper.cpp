#include <gtest/gtest.h>
#include <metl.h>

using namespace metl;

class VariableWrapperFixture: public ::testing::Test
{
public:
	int i = 0;
	metl::UntypedValue<bool, int, double> variable{&i};
};

TEST_F(VariableWrapperFixture, construct)
{
	auto var = metl::UntypedValue<bool, int, double>{&i};
}

TEST_F(VariableWrapperFixture, setValue)
{
	auto newI = 2;
	ASSERT_NO_THROW(variable.setValue(newI));

	ASSERT_EQ(newI, i);
	// variable.setValue(2.0); gives compiler warning for possible  loss of data. no clue how to put that into an assertion
}

TEST_F(VariableWrapperFixture, makeUntypedExpression)
{
	auto expr = variable.makeUntypedExpression();

	const auto typedExpr = expr.get<int>();

	ASSERT_EQ(i, typedExpr());

	i = 2;
	ASSERT_EQ(i, typedExpr());
}
