#pragma once
#include <type_traits>

namespace metl
{
	namespace internal
	{
		template<class ReturnType, class T>
		constexpr ReturnType failingStaticAssert()
		{
			static_assert(std::integral_constant<T, false>::value, "Trying to use something that is not implemented, a base case of some kind");
		}
		
	}
}
