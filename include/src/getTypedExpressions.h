#pragma once

namespace metl
{
	namespace internal
	{
		namespace detail
		{
			template <class... Ts, std::size_t... Ind, class Expression>
			auto getTypedExpressions_impl(const std::vector<Expression>& v, std::index_sequence<Ind...>)
			{
				return std::make_tuple(v.at(Ind).template get<Ts>()...);
			}
		}

		template <class... Ts, class Expression>
		auto getTypedExpressions(const std::vector<Expression>& v)
		{
			return detail::getTypedExpressions_impl<Ts...>(v, std::make_index_sequence<sizeof...(Ts)>{});
		}

		template <class T, class Expression>
		auto getTypedExpressions(const Expression& e)
		{
			return std::make_tuple(e.get<T>());
		}
	}
}