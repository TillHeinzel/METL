
#include <gtest/gtest.h>

#include "metl.h"

#include "src/Utility/is_specialization_of.h"
#include "src/Utility/is_callable.h"
#include "src/Utility/MixedVisitor.h"
#include "src/Utility/AUTO_MEMBER.h"

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

class is_callable_Fixture : public ::testing::Test
{
public:
};

//TEST_F(is_callable_Fixture, failingBaseCase_v)
//{
//	using namespace metl::internal;
//	static_assert(is_callable_v<int, double, bool>, "");
//}

TEST_F(is_callable_Fixture, singleArgNoReturn)
{
	using namespace metl::internal;
	auto f1 = [](int i)
	{
		return i;
	};

	static_assert(decltype(is_callable(CallableType<decltype(f1)>(), Type<int>()))::value, "");
}

TEST_F(is_callable_Fixture, singleArgNoReturn_v)
{
	using namespace metl::internal;
	auto f1 = [](int i)
	{
		return i;
	};

	static_assert(is_callable_v<decltype(f1), Arguments<int>>, "");
}

TEST_F(is_callable_Fixture, complexArgsNoReturn_v)
{
	using namespace metl::internal;
	auto f1 = [](int i, double*, std::string&)
	{
		return i;
	};

	static_assert(is_callable_v<decltype(f1), Arguments<int, double*, std::string&>>, "");
}

TEST_F(is_callable_Fixture, singleArgWithReturn)
{
	using namespace metl::internal;

	auto f1 = [](int i)
	{
		return i;
	};

	static_assert(decltype(is_callable(CallableType<decltype(f1)>(), Return<int>(), Type<int>()))::value, "");
	static_assert(!decltype(is_callable(CallableType<decltype(f1)>(), Return<std::string>(), Type<int>()))::value, "");
}

TEST_F(is_callable_Fixture, singleArgWithReturn_v)
{
	using namespace metl::internal;

	auto f1 = [](int i)
	{
		return i;
	};

	static_assert(is_callable_v<decltype(f1), Return<int>, Arguments<int>>, "");
}


TEST_F(is_callable_Fixture, complexArgsWithReturn_v)
{
	using namespace metl::internal;
	auto f1 = [](int i, double*, std::string&)
	{
		return i;
	};

	static_assert(is_callable_v<decltype(f1), Return<int>, Arguments<int, double*, std::string&>>, "");
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
