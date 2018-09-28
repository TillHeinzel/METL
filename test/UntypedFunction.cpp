
#include <gtest/gtest.h>
#include <complex>

#include "metl.h"

#include "DynamicOperationFixture.h"

using namespace metl;

namespace
{
	auto makeStaticFunction()
	{
		return [](bool b, int i)
		{
			return b ? i : i + 1;
		};
	}
}

class DynamicFunctionFixture : public DynamicOperationFixture
{
public:
	std17::remove_cvref_t<decltype(makeStaticFunction())> f = makeStaticFunction();
};

TEST_F(DynamicFunctionFixture, make)
{
	auto invoke = [&f = f]()
	{
		internal::makeDynamicFunction<Expression, bool, int>(f);
	};

	EXPECT_NO_THROW(invoke());
}

TEST_F(DynamicFunctionFixture, withConstexpr)
{
	auto boolFunction = TypedExpression<bool>([]()
	{
		return true;
	});
	auto boolExpression = Expression{boolFunction, metl::CATEGORY::CONSTEXPR};

	auto dynamicFunction = internal::makeDynamicFunction<Expression, bool, int>(f);

	auto result = dynamicFunction({boolExpression, intExpression});

	EXPECT_TRUE(result.isConstexpr());

	EXPECT_NO_THROW(result.get<int>());

	EXPECT_EQ(0, result.get<int>()());
}

TEST_F(DynamicFunctionFixture, withNonConstexpr)
{
	auto b = true;
	auto boolFunction = TypedExpression<bool>([&b]()
	{
		return b;
	});
	auto boolExpression = Expression{boolFunction, metl::CATEGORY::DYNEXPR};

	auto dynamicFunction = internal::makeDynamicFunction<Expression, bool, int>(f);

	auto result = dynamicFunction({boolExpression, intExpression});

	EXPECT_FALSE(result.isConstexpr());

	EXPECT_NO_THROW(result.get<int>());

	EXPECT_EQ(0, result.get<int>()());

	b = false;

	EXPECT_EQ(1, result.get<int>()());
}
