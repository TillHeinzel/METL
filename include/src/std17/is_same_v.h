#pragma once

#include <type_traits>

namespace std17
{
	template<class T1, class T2>
	constexpr static bool is_same_v = std::is_same<T1, T2>::value;
}