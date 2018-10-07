
#include <gtest/gtest.h>

#include "metl.hpp"

#include "boost/hana.hpp"

#include "src/Utility/AUTO_MEMBER.hpp"

using namespace metl;
namespace hana = boost::hana;

class HanaFixture : public ::testing::Test
{
public:
};

TEST_F(HanaFixture, is_valid)
{
	auto is_callable = hana::is_valid(
		[](auto&& f, auto&&... params) -> decltype((void)f(params...))
	{});

	auto lambda = [](int i)
	{
		return i;
	};

	static_assert(is_callable(lambda, 0), "");
	static_assert(!is_callable(lambda, "str"), "");
}
