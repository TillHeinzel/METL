#pragma once

#include <functional>

#include "DeepCopy.h"

#include "TypeList.h"

namespace metl
{

	template<class T> using exprType = std::function<T()>;

	enum class TYPE : int;

	template<class T, class... Ts>
	constexpr TYPE classToType2()
	{
		return static_cast<TYPE>(findFirstIndex<T>(TypeList<Ts...>{}));
	}

	enum class CATEGORY
	{
		CONSTEXPR, //an expression that does not change after "build"-time, when the string is read. Constexpressions allow for some optimizations.
		DYNEXPR // an expression that DOES change after "build"-time. 
	};


	template<class... Ts, std::size_t... I>
	std::tuple<std::unique_ptr<Ts>...> deepCopy(const std::tuple<std::unique_ptr<Ts>...>& tuple, std::index_sequence<I...>)
	{
		return std::make_tuple<std::unique_ptr<Ts>...>(qengine::internal::deep_copy(std::get<I>(tuple))...);
	}

	template<class... Ts>
	class Expression2
	{
	public:
		template<class T>
		Expression2(const exprType<T>& t, CATEGORY category = CATEGORY::DYNEXPR) :
			type_(classToType2<T, Ts...>()),
			category_(category)
		{
			constexpr auto index = findFirstIndex<T>(TypeList<Ts...>{});
			std::get<index>(vals_) = std::make_unique<exprType<T>>(t);
		}

		Expression2(const Expression2& other) :
			type_(other.type_),
			category_(other.category_)
		{
			vals_ = deepCopy(other.vals_, std::make_index_sequence<sizeof...(Ts)>{});
		}

		Expression2& operator=(const Expression2& other)
		{
			if (&other != this)
			{
				type_ = other.type_;
				category_ = other.category_;
				vals_ = deepCopy(other.vals_, std::make_index_sequence<sizeof...(Ts)>{});
			}

			return *this;
		}

		template<class T> exprType<T> get() const
		{
			constexpr auto index = findFirstIndex<T>(TypeList<Ts...>{});
			static_assert(index < sizeof...(Ts), "Error: Requested Type is not a valid type!");

			if (std::get<index>(vals_)) return *std::get<index>(vals_);
			throw std::runtime_error("this is not the correct type");
		}

		TYPE type() const { return type_; }
		CATEGORY category() const { return category_; }

		template<class T>
		constexpr static TYPE toType() { return classToType2<T, Ts...>(); }
	private:
		TYPE type_;
		CATEGORY category_;

		std::tuple<std::unique_ptr<exprType<Ts>>...> vals_;
	};
}
