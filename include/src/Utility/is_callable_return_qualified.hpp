#pragma once

#include <type_traits>

namespace metl
{
	namespace internal
	{
		/*template<typename C, template<class> class ReturnQualifier, typename... Args>
		struct is_callable_return_qualified 
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

		template<typename T1, typename T2>
		static constexpr bool is_callable_v = is_callable<T1, T2>::value;
*/
	}
}
