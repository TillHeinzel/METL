

#pragma once

#include "src/TypeErasure/UntypedConstant.h"

#include "src/TypeErasure/UntypedExpression.h"

#include "src/std17/remove_cvref.h"

namespace metl
{
	
	template <class ... Ts>
	template <class T>
	UntypedConstant<Ts...>::UntypedConstant(T val): value_(val)
	{
	}

	template <class ... Ts>
	template <class T>
	void UntypedConstant<Ts...>::setValue(const T& t)
	{
		value_ = t;
	}

	template <class ... Ts>
	void UntypedConstant<Ts...>::setValueUntyped(const UntypedConstant& newValue)
	{
		value_ = newValue.value_;
	}

	template <class ... Ts>
	UntypedExpression<Ts...> UntypedConstant<Ts...>::makeUntypedExpression() const
	{
		auto visitor = [](const auto& typedValue)
		{
			return UntypedExpression<Ts...>::makeConstexpr(typedValue);
		};
		return mpark::visit(visitor, value_);
	}

	template <class ... Ts>
	TYPE UntypedConstant<Ts...>::type() const
	{
		auto visitor = [](const auto& typedValue)
		{
			return classToType2<std17::remove_cvref_t<decltype(typedValue)>, Ts...>();
		};
		return mpark::visit(visitor, value_);
	}
}
