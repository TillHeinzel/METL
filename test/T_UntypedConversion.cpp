
#include <gtest/gtest.h>
#include <complex>

#include "metl.hpp"
#include "src/std17/remove_cvref.hpp"

#include "DynamicOperationFixture.hpp"

using namespace metl;

namespace
{
	auto makeStaticFunction()
	{
		return [](int i)
		{
			return static_cast<double>(i);
		};
	}
}

class DynamicConversionFixture : public DynamicOperationFixture
{
public:
	std17::remove_cvref_t<decltype(makeStaticFunction())> f = makeStaticFunction();
};

TEST_F(DynamicConversionFixture, make)
{
	auto invoke = [&f = f]()
	{
		internal::makeDynamicConversion<Expression, int>(f);
	};

	EXPECT_NO_THROW(invoke());
}

TEST_F(DynamicConversionFixture, withConstexpr)
{
	auto dynamicConversion = internal::makeDynamicConversion<Expression, int>(f);

	auto result = dynamicConversion.apply(intExpression);

	EXPECT_TRUE(result.isConstexpr());

	EXPECT_NO_THROW(result.get<double>());

	EXPECT_EQ(0.0, result.get<double>()());
}

TEST_F(DynamicConversionFixture, withNonConstexpr)
{
	auto i = 0;
	auto dynamicIntFunction = TypedExpression<int>([&i]()
	{
		return i;
	});
	auto dynamicIntExpression = Expression::makeNonConstexpr(dynamicIntFunction);

	auto dynamicConversion = internal::makeDynamicConversion<Expression, int>(f);

	auto result = dynamicConversion.apply(dynamicIntExpression);

	EXPECT_FALSE(result.isConstexpr());

	EXPECT_NO_THROW(result.get<double>());

	EXPECT_EQ(0.0, result.get<double>()());

	i = 1;

	EXPECT_EQ(1.0, result.get<double>()());
}
