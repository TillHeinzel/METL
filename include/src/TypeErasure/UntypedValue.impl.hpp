

#pragma once

#include "src/TypeErasure/UntypedValue.hpp"

#include "src/TypeErasure/UntypedExpression.hpp"

namespace metl
{
	template <class ... Ts>
	template <class T>
	UntypedValue<Ts...>::UntypedValue(T val) : value_(UntypedConstant<Ts...>(val))
	{}

	template <class ... Ts>
	template <class T>
	UntypedValue<Ts...>::UntypedValue(T* val) : value_(UntypedVariable<Ts...>(val))
	{}

	template <class ... Ts>
	template <class T>
	void UntypedValue<Ts...>::setValue(const T& t)
	{
		auto visitor = [&t](auto& constantOrVariable)
		{
			constantOrVariable.setValue(t);
		};
		mpark::visit(visitor, value_);
	}

	template <class ... Ts>
	void UntypedValue<Ts...>::setValueUntyped(const UntypedValue<Ts...>& newValue)
	{
		assert(newValue.isConstant());

		const auto& newValueConstant = mpark::get<UntypedConstant<Ts...>>(newValue.value_);

		auto visitor = [&newValueConstant](auto& constantOrVariable)
		{
			constantOrVariable.setValueUntyped(newValueConstant);
		};
		mpark::visit(visitor, value_);
	}

	template <class ... Ts>
	UntypedExpression<Ts...> UntypedValue<Ts...>::makeUntypedExpression() const
	{
		auto visitor = [](const auto& constantOrVariable)
		{
			return constantOrVariable.makeUntypedExpression();
		};
		return mpark::visit(visitor, value_);
	}

	template <class ... Ts>
	TYPE UntypedValue<Ts...>::type() const
	{
		auto visitor = [](const auto& constantOrVariable)
		{
			return constantOrVariable.type();
		};
		return mpark::visit(visitor, value_);
	}

	template <class ... Ts>
	bool UntypedValue<Ts...>::isConstant() const
	{
		return mpark::holds_alternative<UntypedConstant<Ts...>>(value_);
	}

	template <class ... Ts>
	bool UntypedValue<Ts...>::isVariable() const
	{
		return mpark::holds_alternative<UntypedVariable<Ts...>>(value_);
	}
}