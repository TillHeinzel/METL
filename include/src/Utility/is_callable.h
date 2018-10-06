#pragma once

#include <type_traits>

#include "boost/hana.hpp"

#include "src/Utility/AUTO_MEMBER.h"
#include "src/Utility/LabelledType.h"
#include "src/std17/is_same_v.h"

namespace metl
{
	namespace internal
	{
		template<typename, typename T>
		struct is_callable
		{
			static_assert(
				std::integral_constant<T, false>::value,
				"Second template parameter needs to be of function type.");
		};

		template<typename C, typename Ret, typename... Args>
		struct is_callable <C, Ret(Args...)>
		{
		private:
			template<typename T, typename = std::enable_if_t<std::is_same<decltype(std::declval<T>()(std::declval<Args>()...)), Ret>::value>>
			static constexpr bool check(T*)
			{
				return true;
			}

			template<typename>
			static constexpr bool check(...)
			{
				return false;
			}



		public:
			static constexpr bool value = check<C>(0);
		};

		template<class T>
		using Return = LabeledType<T, struct ReturnLabel>;

		template<class T>
		using CallableType = LabeledType<T, struct FunctionLabel, Callable>;

		template<class... Ts>
		using Arguments = LabeledTypeList<TypeList<Ts...>, struct ArgumentsLabel>;

		struct is_callable2_struct
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

		constexpr is_callable2_struct is_callable2;

		template<class F, class, class = void>
		constexpr bool is_callable2_v = false;

		template<class F, class... Params>
		constexpr bool is_callable2_v<F, Arguments<Params...>> = decltype(is_callable2(CallableType<F>(), Type<Params>()...))::value;

		template<class F, class Ret, class... Params>
		constexpr bool is_callable2_v<F, Return<Ret>, Arguments<Params...>> = decltype(is_callable2(CallableType<F>(), Return<Ret>(), Type<Params>()...))::value;

		//template<typename T1, typename T2>
		//static constexpr bool is_callable_v = is_callable<T1, T2>::value;


	}
}
