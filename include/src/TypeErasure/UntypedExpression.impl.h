#pragma once

#include "src/TypeErasure/UntypedExpression.h"

namespace metl
{
	template <class ... Ts>
	template <class T>
	UntypedExpression<Ts...> UntypedExpression<Ts...>::makeConstexpr(T t)
	{
		static_assert(internal::isInList<T, Ts...>(), "cannot construct UntypedExpression with this type");
		auto ExpressionLambda = [t]()
		{
			return t;
		};
		auto typedExpression = TypedExpression<T>{ExpressionLambda};
		return UntypedExpression(typedExpression, true);
	}

	template <class ... Ts>
	template <class T>
	UntypedExpression<Ts...> UntypedExpression<Ts...>::makeNonConstexpr(const TypedExpression<T>& t)
	{
		return UntypedExpression(t, false);
	}

	template <class ... Ts>
	template <class T>
	UntypedExpression<Ts...> UntypedExpression<Ts...>::makeNonConstexpr(const T& t)
	{
		static_assert(canConstructTypedExpressionFrom<T>, "must be able to construct typedExpression from inputType");
		return makeNonConstexpr(TypedExpression<typename std::result_of<T()>::type>(t));
	}

	template <class ... Ts>
	template <class T>
	TypedExpression<T> UntypedExpression<Ts...>::get() const
	{
		static_assert(internal::isInList<T, Ts...>(), "Error: Requested Type is not a valid type!");

		if (mpark::holds_alternative<TypedExpression<T>>(vals_)) return mpark::get<TypedExpression<T>>(vals_);
		throw std::runtime_error("this is not the correct type");
	}

	template <class ... Ts>
	UntypedExpression<Ts...> UntypedExpression<Ts...>::evaluatedExpression() const
	{
		auto visitor = [](const auto& expr)
		{
			return UntypedExpression::makeConstexpr(expr());
		};

		return mpark::visit(visitor, vals_);
	}

	template <class ... Ts>
	UntypedValue<Ts...> UntypedExpression<Ts...>::evaluateUntyped() const
	{
		auto visitor = [](const auto& expr)
		{
			return UntypedValue<Ts...>(expr());
		};

		return mpark::visit(visitor, vals_);
	}

	template <class ... Ts>
	template <class T>
	bool UntypedExpression<Ts...>::isType() const
	{
		return type_ == toType<T>();
	}

	template <class ... Ts>
	bool UntypedExpression<Ts...>::isConstexpr() const
	{
		return isConstexpr_;
	}

	template <class ... Ts>
	TYPE UntypedExpression<Ts...>::type() const
	{
		return type_;
	}

	template <class ... Ts>
	template <class T, std::enable_if_t<!internal::isInList<T, Ts...>(), int>>
	UntypedExpression<Ts...>::UntypedExpression(const TypedExpression<T>&, bool)
	{
		static_assert(internal::isInList<T, Ts...>(), "cannot construct UntypedExpression with this type");
	}

	template <class ... Ts>
	template <class T, std::enable_if_t<internal::isInList<T, Ts...>(), int>>
	UntypedExpression<Ts...>::UntypedExpression(const TypedExpression<T>& t, bool isConstexpr):
		type_(classToType2<T, Ts...>()),
		isConstexpr_(isConstexpr),
		vals_(t)
	{
	}
}
