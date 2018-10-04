
#include <gtest/gtest.h>
#include <complex>

#include "metl.h"

#include "src/Utility/is_specialization_of.h"
#include "src/Utility/is_callable.h"
#include "src/Utility/MixedVisitor.h"

#include "AUTO_MEMBER.h"

using namespace metl;


class UtilityFixture : public ::testing::Test
{
public:
};

TEST_F(UtilityFixture, is_specialization_of)
{
	using Tup = std::tuple<int, bool, double>;

	static_assert(internal::is_specialization_of<Tup, std::tuple>::value, "");
	static_assert(!internal::is_specialization_of<Tup, UntypedExpression>::value, "");

	using Expr = UntypedExpression<int, bool, double>;

	static_assert(!internal::is_specialization_of<Expr, std::tuple>::value, "");
	static_assert(internal::is_specialization_of<Expr, UntypedExpression>::value, "");

}

TEST_F(UtilityFixture, is_callable)
{
	auto f1 = [](int i)
	{
		return i;
	};


	static_assert(internal::is_callable_v<decltype(f1), int(int)>, "");
	static_assert(!internal::is_callable_v<decltype(f1), int(std::string)>, "");

	auto f2 = [](auto i)
	{
		return i;
	};

	static_assert(internal::is_callable_v<decltype(f2), int(int)>, "");
	static_assert(internal::is_callable_v<decltype(f2), std::string(std::string)>, "");
}

auto makeGenericVisitor()
	{
		return [](const auto&)
		{
			return 0;
		};
	}

class MixedVisitorFixture : public UtilityFixture
{
public:
	class FixedTypeVisitor
	{
		int operator() (const int i) const
		{
			return i;
		}
	};
	FixedTypeVisitor fixedVisitor{};

	AUTO_MEMBER(genericVisitor, makeGenericVisitor());
	AUTO_MEMBER(visitor, internal::makeMixedVisitor(fixedVisitor, genericVisitor));
};

TEST_F(MixedVisitorFixture, monostate)
{
	auto monostate = mpark::variant<mpark::monostate, int, double>{};
	EXPECT_EQ(0, mpark::visit(visitor, monostate));
}

TEST_F(MixedVisitorFixture, doublestate)
{
	auto doublestate = mpark::variant<mpark::monostate, int, double>{2.0};
	EXPECT_EQ(0, mpark::visit(visitor, doublestate));
}

TEST_F(MixedVisitorFixture, intstate)
{
	auto intstate = mpark::variant<mpark::monostate, int, double>{3};
	EXPECT_EQ(3, mpark::visit(visitor, intstate));
}
