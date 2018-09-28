#pragma once


class DynamicOperationFixture : public ::testing::Test
{
public:
	metl::TypedExpression<int> intFunction{[]()->int
	{
		return 0;
	}};

	using Expression = metl::UntypedExpression<bool, int, double>;
	Expression intExpression{intFunction, metl::CATEGORY::CONSTEXPR};
};
