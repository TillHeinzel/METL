#pragma once

#include <type_traits>

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
			};

		public:
			static constexpr bool value = check<C>(0);
		};

		template<typename T1, typename T2>
		static constexpr bool is_callable_v = is_callable<T1, T2>::value;

	}
}
