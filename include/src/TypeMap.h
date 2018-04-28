#pragma once

#include <tuple>

#include "TypeList.h"

namespace metl
{
	namespace internal
	{
		template<class, class, class SFINAE = void> class TypeMap;

		// todo: put in check for uniqueness of keys!
		template<class... Keys, class... ValueTypes>
		class TypeMap<std::tuple<Keys...>, std::tuple<ValueTypes...>, std::enable_if_t<sizeof...(Keys) == sizeof...(ValueTypes)>>
		{
		public:
			TypeMap(std::tuple<ValueTypes...> values) : values_(values) {}

			template<class T>
			auto get() const
			{
				static_assert(isInList<T>(), "requested type is not in list");
				return std::get<internal::findFirstIndex<T>(keys())>(values_);
			}

			template<class T>
			static constexpr bool isInList() { return internal::isInList<T, Keys...>(); }

			std::tuple<ValueTypes...> values() const { return values_; }
			constexpr static TypeList<Keys...> keys() { return TypeList<Keys...>{}; }

		private:
			std::tuple<ValueTypes...> values_;
		};

		template<class... Keys, class... ValueTypes>
		TypeMap<std::tuple<Keys...>, std::tuple<ValueTypes...>> makeTypeMap(TypeList<Keys...>, std::tuple<ValueTypes...> values)
		{
			return TypeMap<std::tuple<Keys...>, std::tuple<ValueTypes...>>(values);
		}

		template<class... Keys, class... ValueTypes>
		constexpr TypeList<Keys...> getKeys(Type<TypeMap<std::tuple<Keys...>, std::tuple<ValueTypes...>>>)
		{
			return TypeList<Keys...>();
		}
	}
}
