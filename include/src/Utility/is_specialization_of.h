#pragma once

#include <type_traits>

namespace metl
{
	namespace internal
	{
		template<class, template<class...> class>
		struct is_specialization_of: std::false_type{};

		template<template<class...> class Templ, class... Ts>
		struct is_specialization_of<Templ<Ts...>, Templ> : std::true_type{};

		template<class T, template<class...> class Temp>
		constexpr static bool is_specialization_of_v = is_specialization_of<T, Temp>::value;
		
	}
}