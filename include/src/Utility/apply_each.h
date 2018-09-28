#pragma once

#include <tuple>

namespace metl
{
	namespace internal
	{
		namespace detail
		{
			template<class F, class Tuple, std::size_t... I>
			decltype(auto) apply_each_impl(F&& f, Tuple&& tuple, std::index_sequence<I...>)
			{
				return std::make_tuple(std::forward<F>(f)(std::get<I>(std::forward<Tuple>(tuple)))...);
			}

		}

		template<class F, class Tuple>
		decltype(auto) apply_each(F&& f, Tuple&& tuple)
		{
			return detail::apply_each_impl(std::forward<F>(f), std::forward<Tuple>(tuple),
				std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{});
		}
	}
}
