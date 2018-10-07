#pragma once

#include <type_traits>

namespace metl
{
	namespace internal
	{
		template<class T>
		struct Callable
		{
			template<class... Args>
			auto operator() (Type<Args>... args) ->decltype(std::declval<T>()(std::declval<Args>()...));

			template<class... Args>
			auto operator() (Type<Args>... args) const ->decltype(std::declval<const T>()(std::declval<Args>()...));
		};

		template<class T, class Label, template<class> class... Functionalities>
		struct LabeledType: public Functionalities<T>...
		{
			using type = T;
			using label = Label;
		};

		template<class T, class>
		struct LabeledTypeList
		{
			static_assert(std::integral_constant<T, false>::value, "first type must be a typelist");
		};

		template<class... Ts, class Label>
		struct LabeledTypeList<TypeList<Ts...>, Label>
		{
			using types = TypeList<Ts...>;
			using label = Label;
		};
	}
}