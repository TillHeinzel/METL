#pragma once

#include <type_traits>

#include "boost/hana.hpp"

#include "src/Utility/LabelledType.h"
#include "src/std17/is_same_v.h"
#include "src/Utility/failingStaticAssert.h"

namespace metl
{
	namespace internal
	{
		template<class T>
		using Return = LabeledType<T, struct ReturnLabel>;

		template<class T>
		using CallableType = LabeledType<T, struct FunctionLabel, Callable>;

		template<class... Ts>
		using Arguments = LabeledTypeList<TypeList<Ts...>, struct ArgumentsLabel>;

		struct is_callable_struct
		{
			template<class F, class... Args>
			constexpr static auto isCallPossible(CallableType<F> f, Type<Args>... args)
			{

				return boost::hana::is_valid(
					[](auto f, auto... params) -> decltype((void)f(params...))
				{})
					(f, args...);
			}


			template<class F, class... Args>
			constexpr auto operator() (CallableType<F> f, Type<Args>... args) const
			{
				return isCallPossible(f, args...);
			}

			template<class F, class Ret, class... Args>
			constexpr auto operator() (CallableType<F> f, Return<Ret>, Type<Args>... args) const
			{
				return boost::hana::eval_if(
					isCallPossible(f, args...),
					[f, args...](auto _)
				{
					return boost::hana::bool_c<std17::is_same_v<Ret, decltype(_(f)(args...))>>;
				},
					[](auto)
				{
					return boost::hana::false_c;
				}
				);
			}
		};

		constexpr is_callable_struct is_callable;
		
		template<class F, class, class = void>
		constexpr bool is_callable_v = failingStaticAssert<bool, F>();

		template<class F, class... Params>
		constexpr bool is_callable_v<F, Arguments<Params...>> = decltype(is_callable(CallableType<F>(), Type<Params>()...))::value;

		template<class F, class Ret, class... Params>
		constexpr bool is_callable_v<F, Return<Ret>, Arguments<Params...>> = decltype(is_callable(CallableType<F>(), Return<Ret>(), Type<Params>()...))::value;

	}
}
