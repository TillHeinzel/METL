#pragma once

#include "nameMangling.h"
#include "VarExpression.h"

namespace metl
{
	template<class... Ts, std::size_t... Ind, class Expression>
	auto getTypedExpr(const std::vector<Expression>& v, std::index_sequence<Ind...>)
	{
		return std::make_tuple(v.at(Ind).template get<Ts>()...);
	}

	template <class F, class Tuple, std::size_t... I>
	constexpr decltype(auto) apply(F&& f, Tuple&& t, std::index_sequence<I...>)
	{
		return f(std::get<I>(std::forward<Tuple>(t))...);
	}

	template<class Tuple, std::size_t... I>
	auto evaluate(Tuple&& funcs, std::index_sequence<I...>)
	{
		return std::make_tuple(std::get<I>(funcs)()...);
	}

	template<class Expression, class... Ts, class F>
	FunctionImpl<Expression> makeFunction(const F& f)
	{
		return FunctionImpl<Expression>
		(
			[f](const std::vector<Expression>& v)
			{
				auto vv = getTypedExpr<Ts...>(v, std::make_index_sequence<sizeof...(Ts)>{});

				using retType = std::result_of_t<F(Ts...)>;
				return Expression(exprType<retType>([f, vv]()
				{
					auto vals = evaluate(vv, std::make_index_sequence<sizeof...(Ts)>{});
					return apply(f, vals, std::make_index_sequence<sizeof...(Ts)>{});
				}));
			}
		);
	}

	template<class Expression, class T>
	Expression makeVariableExpression(T* v)
	{
		auto f = [v]()
		{
			return *v;
		};

		return Expression(exprType<T>(f), CATEGORY::DYNEXPR);
	}
}

