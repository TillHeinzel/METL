#pragma once

namespace metl
{
	namespace detail
	{
		template<class TrueBranch, class FalseBranch>
		decltype(auto) constexpr constexpr_if_invoke(const std::true_type&, const TrueBranch& trueBranch, const FalseBranch&)
		{
			return trueBranch([](const auto& t) {return t; });
		}

		template<class TrueBranch, class FalseBranch>
		decltype(auto) constexpr constexpr_if_invoke(const std::false_type&, const TrueBranch&, const FalseBranch& falseBranch)
		{
			return falseBranch([](const auto& t) {return t; });
		}
	}
	template<class Condition, class TrueBranch, class FalseBranch>
	decltype(auto) constexpr constexpr_if(const Condition& condition, const TrueBranch& trueBranch, const FalseBranch& falseBranch)
	{
		return detail::constexpr_if_invoke(std::integral_constant<bool, Condition::value>{}, trueBranch, falseBranch);
	}
}

namespace metl
{

	template<class...> struct TypeList { constexpr TypeList() = default; };

	template<size_t I, class ToFind>
	constexpr size_t findFirstIndex()
	{
		return I;
	}

	template<size_t I, class ToFind, class T, class... Ts>
	constexpr size_t findFirstIndex()
	{
		return std::is_same<ToFind, T>::value ? I : findFirstIndex<I + 1, ToFind, Ts...>();
	}

	template<class ToFind, class... Ts>
	constexpr size_t findFirstIndex(TypeList<Ts...>)
	{
		return findFirstIndex<0, ToFind, Ts...>();
	}

	template<class ToFind, class... Ts>
	constexpr bool isInList()
	{
		return findFirstIndex<ToFind>(TypeList<Ts...>{}) < sizeof...(Ts);
	}

	template<unsigned index, class... Ts>
	struct Get
	{
		using type = std::remove_cv_t<std::remove_reference_t<decltype(std::get<index>(std::tuple<Ts...>(std::declval<Ts>()...)))>>;
	};


	template<unsigned index, class... Ts> using Get_t = typename Get<index, Ts...>::type;

}

namespace metl
{
	template<class T>
	struct Type
	{
		using type = T;
	};
}
